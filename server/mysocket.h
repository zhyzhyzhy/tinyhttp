//
// Created by zhy on 2/21/17.
//

#ifndef WEBSERVER_SOCKET_H
#define WEBSERVER_SOCKET_H
struct sockaddr_in get_conn_info(int connfd);
int socket_bind_listen(char *addr, char *port);
int set_no_blocking(int fd);
int set_blocking(int fd);
typedef struct {
    int status_code;
    char *status_message;
}http_status_pair;
#endif //WEBSERVER_SOCKET_H
