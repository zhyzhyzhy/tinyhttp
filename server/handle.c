//
// Created by zhy on 2/21/17.
//

#include "handle.h"
#include "log.h"
#include "util.h"
#include "threadpool.h"
#include "config.h"
#include "parser.h"
#include "mysocket.h"
#include "mempool.h"
#include "helper.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event.h>
#include <event2/listener.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdlib.h>


extern int *notify;
extern threadpool_t *threadpool;
extern server_config config;

//the count of the request
//for the use of allocation to subReactor
//see #on_accept
int count;

/**
 * the callback of the accept event
 * send the connect fd to subReactor
 * @param listen_fd the fd that listen on accept event
 * @param events    the bits that represents listen_fd events
 * @param arg       is NULL in this method
 */
void on_accept(int listen_fd, short events, void *arg) {
    int conn_fd = accept(listen_fd, NULL, 0);
    //log_info("accept %d\n", conn_fd);
    // log_info("count %d\n", count);
    if (conn_fd < 0) {
        return;
    }
    set_no_blocking(conn_fd);
    debug("the count is %d ", count);
    int index = ++count % config.sub_reactor;
    non_blocking_write(notify[index], (char *) &conn_fd, sizeof(int));
}


/**
 * the callback of read event
 * @param conn_fd  the client connection fd
 * @param event
 * @param arg      struct http_request*
 */
void on_read(int conn_fd, short event, void *arg) {
    struct http_request *request = (struct http_request *) arg;
    thread_job *current_job = (thread_job *) mmalloc(sizeof(thread_job));
    current_job->next = NULL;
    current_job->func = process_request;
    current_job->arg = request;
    add_job(threadpool, current_job);
}

/**
 * process the request
 * @param arg  struct http_request point
 */
void process_request(void *arg) {
    char method[24];
    char path[1024];
    char version[24];

    memset(method, 0, 24);
    memset(path, 0, 1024);
    memset(version, 0, 24);

    char request_head[8192];
    memset(request_head, 0, 8192);

    struct http_request *request = (struct http_request *) arg;
    int conn_fd = request->connfd;

    int has_read = 0;
    for (;;) {
        ssize_t receive_count = recv(conn_fd, request_head + has_read, 8192 - has_read, 0);
        has_read += receive_count;
        if (receive_count > 0) {
            //防止读半包问题，如果没有读完，则继续读
            if (!check_read_done(request_head)) {
                continue;
            } else {
              //  struct sockaddr_in client = get_conn_info(conn_fd);
                sscanf(request_head, "%s %s %s", method, path, version);
                break;
            }
        } else if (receive_count < 0 && errno != EAGAIN) {
            return;
        } else {
            //receive_count < 0 encounter error
            //or receive count == 0 close connection
            //close fd
            event_del(request->pread);
            event_free(request->pread);
            mfree(request);
            close(conn_fd);
            //log_info("close %d", conn_fd);
            return;
        }

    }

    strcpy(request->method, method);
    url_decode(path, 1024, request->path);
    strcpy(request->version, version);

//    struct sockaddr_in client_info = get_conn_info(conn_fd);
//    log("%s\t %s\t %-24s\t %s\t", inet_ntoa(client_info.sin_addr), request->method, request->path, request->version);

    if (strncmp(method, "GET", 3) == 0) {
        if (is_end_with_ch(path, '/')) {
            char str[1024];
            memset(str, 0, 1024);
            strcpy(str, path);
            strcat(str, "index.html");
            do_get(conn_fd, str + 1, "text/html");
        } else {
            do_get(conn_fd, path + 1, parse_mime_type(path));
        }
    } else {
        server_error(conn_fd, 405);
    }
}


/**
 * response error
 * @param conn_fd  the client id
 * @param status   the response http code
 */
void server_error(int conn_fd, int status) {
    char header[1024];
    memset(header, 0, 1024);
    const char *status_message = parse_http_code(status);
    sprintf(header, "HTTP/1.1 %d %s\r\n", status, status_message);
    sprintf(header, "%sContent-type:text/html\r\n", header);

    const char *error_message = NULL;
    if (status == 404) {
        error_message = "<html>\n"
                "<head><title>404 Not Found</title></head>\n"
                "<body bgcolor=\"white\">\n"
                "<center><h1>404 Not Found</h1></center>\n"
                "<hr>"
                "</body>\n"
                "</html>";
        sprintf(header, "%sContent-length: %lu\r\n\r\n", header, strlen(error_message));
    } else if (status == 405) {
        error_message = "<html>\n"
                "    <head>\n"
                "        <title>405 Not Allowed</title>\n"
                "    </head>\n"
                "    <body bgcolor=\"white\">\n"
                "        <center>\n"
                "            <h1>405 Not Allowed</h1>\n"
                "        </center>\n"
                "        <hr>\n"
                "    </body>\n"
                "</html>";
        sprintf(header, "%sContent-length: %lu\r\n\r\n", header, strlen(error_message));
    }
    non_blocking_write(conn_fd, header, (int) strlen(header));
    non_blocking_write(conn_fd, error_message, (int) strlen(error_message));
}

void do_get(int conn_fd, char *file_name, const char *file_type) {
    char buffer[16392];
    int fd;

    struct stat file;
    if (strcmp(file_name, "") == 0) {
        file_name = config.index_file_name;
        fd = open(file_name, O_RDONLY);
    } else {
        fd = open(file_name, O_RDONLY);
    }
    if (fd == -1) {
        server_error(conn_fd, 404);
        return;
    }
    if (stat(file_name, &file) == -1) {
        log_err("failed open %s\n", file_name);
        close(fd);
        return;
    }

    // send headers
    char header[1024];
    sprintf(header, "HTTP/1.1 200 ok\r\n");
    sprintf(header, "%sContent-Length: %lld\r\n", header, file.st_size);
    sprintf(header, "%sConnection: keep-alive\r\n", header);
    sprintf(header, "%sServer: zhuyichen's server\r\n", header);
    sprintf(header, "%sContent-Type: %s\r\n\r\n", header, file_type);

    non_blocking_write(conn_fd, header, (int) strlen(header));

    // send body
    int byte_count = 0;
    while ((byte_count = (int) read(fd, buffer, 16392)) > 0) {
        non_blocking_write(conn_fd, buffer, byte_count);
    }

    //close file fd;
    close(fd);
}

void on_timeout(int connfd, short event, void *arg) {
    struct http_request *request = (struct http_request *) arg;
    event_del(request->timeout);
    mfree(request->tv);
}