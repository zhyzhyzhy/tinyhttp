//
// Created by zhy on 2/21/17.
//

#include "handle.h"
#include "mysocket.h"
#include "log.h"
#include <stdio.h>
#include <string.h>
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

extern struct event_base *base;
http_status_pair http_status[] = {
        {200, "OK"},
        {404, "Not Found"},
        {NULL, NULL}
};

extern int count;
void do_get(int connfd, char *file_name, char *file_type) {
    char buffer[8192];
    int fd;
    if (strncmp(file_name, "", strlen(file_name)) == 0) {
        fd = open("index.html", O_RDONLY);
    } else {
        fd = open(file_name, O_RDONLY);
    }
    if (fd == -1) {
        server_error(connfd, 404);
        return;
    }
//    log_info("%d\n", count++);
    char header[1024];
    sprintf(header, "HTTP/1.1 200 ok\r\n");

    sprintf(header, "%sContent-type: %s\r\n", header, file_type);


    int length;
//    if ((length = (int) read(fd, buffer, 8192)) > 0) {
    length = (int) read(fd, buffer, 8192);
    sprintf(header, "%sContent-length: %d\r\n\r\n", header, length);
    send(connfd, header, strlen(header), 0);
    send(connfd, buffer, (size_t) length, 0);
//        log_info("send message\n");
//    }

//    log_info("close fd %d\n\n", connfd);
    close(fd);
//    close(connfd);
}

void on_accept(int serverfd, short events, void *arg) {

    char method[24];
    char path[1024];
    char version[24];

    memset(method, 0, 24);
    memset(path, 0, 1024);
    memset(version, 0, 24);

    char request[8192];

    int connfd = accept(serverfd, NULL, 0);



    set_no_blocking(connfd);

    struct event *pread = (struct event *) malloc(sizeof(struct event));
    struct event *pwrite = (struct event *) malloc(sizeof(struct event));
    struct request *request1 = (struct request *) malloc(sizeof(request));

    request1->pread = pread;
    request1->pwrite = pwrite;

    event_set(pread, connfd, EV_READ | EV_PERSIST, on_read, request1);
    event_base_set(base, pread);
    event_add(pread, NULL);
}


void on_read(int connfd, short ievent, void *arg) {

    char method[24];
    char path[1024];
    char version[24];

    memset(method, 0, 24);
    memset(path, 0, 1024);
    memset(version, 0, 24);

    char request[8192];

    struct request *request1 = (struct request*)arg;

//    struct event *pwrite = (struct event *) malloc(sizeof(struct event));
    if (recv(connfd, request, 8192, 0) > 0) {
//        printf(request);
        struct sockaddr_in client = get_conn_info(connfd);
        sscanf(request, "%s %s %s", method, path, version);
        printf("get connect from :  %s\n", inet_ntoa(client.sin_addr));
        log_info("response fd %d\n", connfd);
        printf("method : %s\npath : %s\nversion : %s\n\n", method, path, version);
    }
    else {
        event_del(request1->pread);
        event_del(request1->pwrite);
        free(request1->pread);
        free(request1->pwrite);
        free(request1);
        log_info("0000close fd %d\n", connfd);
        close(connfd);
        return;
    }

    strcpy(request1->method, method);
    strcpy(request1->path, path);
    strcpy(request1->version, version);



    event_set(request1->pwrite, connfd, EV_WRITE, on_write, request1);
    event_base_set(base, request1->pwrite);
    event_add(request1->pwrite, NULL);
}

void on_write(int connfd, short ievent, void *arg) {

    char method[24];
    char path[1024];
    char version[24];

    memset(method, 0, 24);
    memset(path, 0, 1024);
    memset(version, 0, 24);
    struct request *request1 = (struct request*)arg;

    strcpy(method, request1->method);
    strcpy(path, request1->path);
    strcpy(version, request1->version);

    printf("method : %s\npath : %s\nversion : %s\n", method, path, version);
//        printf("response fd = %d\n\n", connfd);
    if (strncmp(method, "GET", 3) == 0) {
        if (strstr(path, ".html") != NULL || strncmp(path + 1, "", strlen(path) - 1) == 0) {
            do_get(connfd, path + 1, "text/html");
        } else if (strstr(path, ".js") != NULL) {
            do_get(connfd, path + 1, "text/js");
        } else if (strstr(path, ".css") != NULL) {
            do_get(connfd, path + 1, "text/css");
        } else if (strstr(path, ".xml") != NULL) {
            do_get(connfd, path + 1, "text/xml");
        } else if (strstr(path, ".xhtml") != NULL) {
            do_get(connfd, path + 1, "application/xhtml+xml");
        } else if (strstr(path, ".png") != NULL) {
            do_get(connfd, path + 1, "image/png");
        } else if (strstr(path, ".gif") != NULL) {
            do_get(connfd, path + 1, "image/gif");
        } else if (strstr(path, ".jpg") != NULL) {
            do_get(connfd, path + 1, "image/jpg");
        } else if (strstr(path, ".jpeg") != NULL) {
            do_get(connfd, path + 1, "image/jpeg");
        } else if (strstr(path, ".jpeg") != NULL) {
            do_get(connfd, path + 1, "image/jpeg");
        } else if (strstr(path, ".woff") || strstr(path, ".ttf")) {
            do_get(connfd, path + 1, "application/octet-stream");
        } else
            do_get(connfd, path + 1, "text/plain");
    }
//    event_del(request1->pwrite);
//    event_del(request1->pread);
//    free(request1->pread);
//    free(request1->pwrite);
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

void server_error(int connfd, int status) {
    char header[1024];
    memset(header, 0, 1024);
    char *status_message = parse_http_code(status);
    sprintf(header, "HTTP/1.1 %d %s\r\n", status, status_message);
    sprintf(header, "%sContent-type:text/html\r\n\r\n", header);

    send(connfd, header, strlen(header), 0);

    if (status == 404) {
        char *error_message = "<html>\n"
                "<head><title>404 Not Found</title></head>\n"
                "<body bgcolor=\"white\">\n"
                "<center><h1>404 Not Found</h1></center>\n"
                "<hr>"
                "</body>\n"
                "</html>";
        send(connfd, error_message, strlen(error_message), 0);
    }
}
