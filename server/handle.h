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
void on_write(int connfd,short ievent,void *arg);

void on_read1(int connfd,short ievent,void *arg);
void on_accept1(int serverfd,short events, void* arg);
struct request {
    char method[24];
    char path[1024];
    char version[24];
    struct event *pread;
    struct event *pwrite;
    struct event_base *base;
};
#endif //WEBSERVER_HANDLE_H
