//
// Created by zhy on 2/21/17.
//

#include "handle.h"
#include "mysocket.h"
#include "log.h"
#include "helper.h"
#include "util.h"
#include "threadpool.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdlib.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event.h>
#include <event2/listener.h>
#include <sys/stat.h>


extern int count;
extern int *notify;
extern threadpool_t *threadpool;
http_status_pair http_status[] = {
        {200, "OK"},
        {404, "Not Found"},
        {405, "Method Not Allowed"},
        {100, NULL}
};

void on_accept(int listen_fd, short events, void *arg) {
    int conn_fd = accept(listen_fd, NULL, 0);

    if (conn_fd < 0) {
        return;
    }
    set_no_blocking(conn_fd);
    int index = ++count % 8;
    non_blocking_write(notify[index], (char *) &conn_fd, sizeof(int));
}


void on_read(int conn_fd, short event, void *arg) {

    char method[24];
    char path[1024];
    char version[24];

    memset(method, 0, 24);
    memset(path, 0, 1024);
    memset(version, 0, 24);

    char request_head[8192];

    struct http_request *request = (struct http_request*)arg;

    ssize_t receive_count = recv(conn_fd, request_head, 8192, 0);
    if (receive_count > 0) {
        struct sockaddr_in client = get_conn_info(conn_fd);
        sscanf(request_head, "%s %s %s", method, path, version);
        log("[connect : %s]", inet_ntoa(client.sin_addr));
        log("[response fd %d]", conn_fd);
    }
    else if (receive_count < 0 && errno != EAGAIN){
        return;
    }
    else {
        //receive_count < 0 encounter error
        //or receive count == 0 close connection
        //close fd
        event_del(request->pread);
        event_free(request->pread);
//        event_free(request->pwrite);
        free(request);
        log("close fd %d\n", conn_fd);
        close(conn_fd);
        return;
    }

    strcpy(request->method, method);
    url_decode(path, 1024, request->path, 1024);
    strcpy(request->version, version);

    log("[%s\t%s\t%s\t]", method, request->path, version);

//    event_base_set(request->base, request->pwrite);
//    event_add(request->pwrite, NULL);
    job *job1 = (job*)malloc(sizeof(job));
    job1->next = NULL;
    job1->func = on_demo;
    job1->arg = request;
    add_job(threadpool,job1);
    printf("add jobs\n");
}

void on_demo(void *arg) {
    struct http_request *request = (struct http_request*)arg;
    char *method = request->method;
    char *path = request->path;
    char *version = request->version;
    int conn_fd = request->connfd;
    printf("cdcdcd %d\n", conn_fd);

    if (strncmp(method, "GET", 3) == 0) {
        if (strstr(path, ".html") != NULL || strncmp(path + 1, "", strlen(path) - 1) == 0) {
            do_get(conn_fd, path + 1, "text/html");
        } else if (strstr(path, ".js") != NULL) {
            do_get(conn_fd, path + 1, "text/js");
        } else if (strstr(path, ".css") != NULL) {
            do_get(conn_fd, path + 1, "text/css");
        } else if (strstr(path, ".xml") != NULL) {
            do_get(conn_fd, path + 1, "text/xml");
        } else if (strstr(path, ".xhtml") != NULL) {
            do_get(conn_fd, path + 1, "application/xhtml+xml");
        } else if (strstr(path, ".png") != NULL) {
            do_get(conn_fd, path + 1, "image/png");
        } else if (strstr(path, ".gif") != NULL) {
            do_get(conn_fd, path + 1, "image/gif");
        } else if (strstr(path, ".jpg") != NULL) {
            do_get(conn_fd, path + 1, "image/jpg");
        } else if (strstr(path, ".jpeg") != NULL) {
            do_get(conn_fd, path + 1, "image/jpeg");
        } else if (strstr(path, ".jpeg") != NULL) {
            do_get(conn_fd, path + 1, "image/jpeg");
        } else if (strstr(path, ".woff") || strstr(path, ".ttf")) {
            do_get(conn_fd, path + 1, "application/octet-stream");
        } else {
            do_get(conn_fd, path + 1, "text/plain");
        }
    } else {
        server_error(conn_fd, 405);
    }
}

void on_write(int conn_fd, short event, void *arg) {

//

    struct http_request *request = (struct http_request*)arg;
    char *method = request->method;
    char *path = request->path;
    char *version = request->version;

    if (strncmp(method, "GET", 3) == 0) {
        if (strstr(path, ".html") != NULL || strncmp(path + 1, "", strlen(path) - 1) == 0) {
            do_get(conn_fd, path + 1, "text/html");
        } else if (strstr(path, ".js") != NULL) {
            do_get(conn_fd, path + 1, "text/js");
        } else if (strstr(path, ".css") != NULL) {
            do_get(conn_fd, path + 1, "text/css");
        } else if (strstr(path, ".xml") != NULL) {
            do_get(conn_fd, path + 1, "text/xml");
        } else if (strstr(path, ".xhtml") != NULL) {
            do_get(conn_fd, path + 1, "application/xhtml+xml");
        } else if (strstr(path, ".png") != NULL) {
            do_get(conn_fd, path + 1, "image/png");
        } else if (strstr(path, ".gif") != NULL) {
            do_get(conn_fd, path + 1, "image/gif");
        } else if (strstr(path, ".jpg") != NULL) {
            do_get(conn_fd, path + 1, "image/jpg");
        } else if (strstr(path, ".jpeg") != NULL) {
            do_get(conn_fd, path + 1, "image/jpeg");
        } else if (strstr(path, ".jpeg") != NULL) {
            do_get(conn_fd, path + 1, "image/jpeg");
        } else if (strstr(path, ".woff") || strstr(path, ".ttf")) {
            do_get(conn_fd, path + 1, "application/octet-stream");
        } else {
            do_get(conn_fd, path + 1, "text/plain");
        }
    } else {
        server_error(conn_fd, 405);
    }
    event_del(request->pwrite);
}

char *parse_http_code(int status) {
    http_status_pair pair;
    int i = 0;
    pair = http_status[i];
    while (pair.status_message != NULL) {
        if (pair.status_code == status) {
            return pair.status_message;
        }
        pair = http_status[++i];
    }
    return NULL;
}

void server_error(int conn_fd, int status) {
    char header[1024];
    memset(header, 0, 1024);
    char *status_message = parse_http_code(status);
    sprintf(header, "HTTP/1.1 %d %s\r\n", status, status_message);
    sprintf(header, "%sContent-type:text/html\r\n", header);

    char *error_message = NULL;
    if (status == 404) {
        error_message = "<html>\n"
                "<head><title>404 Not Found</title></head>\n"
                "<body bgcolor=\"white\">\n"
                "<center><h1>404 Not Found</h1></center>\n"
                "<hr>"
                "</body>\n"
                "</html>";
        sprintf(header, "%sContent-length: %lu\r\n\r\n", header, strlen(error_message));
    }
    else if (status == 405) {
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

void do_get(int conn_fd, char *file_name, char *file_type) {
    char buffer[8192];
    int fd;

    struct stat file;
//    if (strncmp(file_name, "", strlen(file_name)) == 0) {
    if (strcmp(file_name, "") == 0) {
        file_name = "index.html";
        fd = open("index.html", O_RDONLY);
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
    sprintf(header, "%sContent-Length: %d\r\n", header, file.st_size);
    sprintf(header, "%sContent-Type: %s\r\n\r\n", header, file_type);

    non_blocking_write(conn_fd, header, (int) strlen(header));

    // send body
    int byte_count = 0;
    while ((byte_count = (int) read(fd, buffer, 8192)) > 0) {
        non_blocking_write(conn_fd, buffer, byte_count);
    }

    //close file fd;
    close(fd);
}