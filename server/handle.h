//
// Created by zhy on 2/21/17.
//

#ifndef WEBSERVER_HANDLE_H
#define WEBSERVER_HANDLE_H
void do_get(int connfd, char* file_name, char* file_type);
void on_accept(int serverfd,short events, void* arg);
void server_error(int connfd, int status);
char* parse_http_code(int status);
void on_read(int connfd,short ievent,void *arg);

void on_demo(void *arg);

void on_timeout(int connfd, short event, void *arg);
struct http_request {
    int connfd;
    char method[24];
    char path[1024];
    char version[24];
    struct event *pread;
    struct event *pwrite;
    struct event *timeout;
    struct timeval* tv;
    struct event_base *base;
};
#endif //WEBSERVER_HANDLE_H
