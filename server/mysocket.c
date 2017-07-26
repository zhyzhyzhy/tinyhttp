//
// Created by zhy on 2/21/17.
//

#include "mysocket.h"
#include "log.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <event.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <event2/event.h>
#include <event.h>
//内核监听队列的最大长度
#define BACKLOG 1024

struct sockaddr_in get_conn_info(int connfd) {
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    getpeername(connfd, (struct sockaddr *) &client, &len);
    return client;
}

int socket_bind_listen(char *addr, char *port) {
    int server_fd;
    unsigned short Port;
    struct sockaddr_in addr_s;
    //AF_INET ipv4
    //SOCK_STREAM 以流的方式传递
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        log_err("socket error\n");
        return -1;
    }

    //get port
    Port = (unsigned short) atoi(port);

    memset(&addr_s, 0, sizeof(addr_s));
    addr_s.sin_family = AF_INET;
    addr_s.sin_addr.s_addr = inet_addr(addr);
    addr_s.sin_port = htons(Port);

    int ret = bind(server_fd, (struct sockaddr *) &addr_s, sizeof(addr_s));
    if (ret == -1) {
        log_err("binding error\n");
        return -1;
    }

    if (listen(server_fd, BACKLOG) == -1) {
        log_err("listen error");
        return -1;
    }
    return server_fd;
}

int set_no_blocking(int fd) {
    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0)) == -1) {
        log_err("fcntl error\n");
        exit(1);
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        log_err("fcntl error\n");
        exit(1);
    }
//    evutil_make_socket_nonblocking(fd);
    return 1;
}

int set_blocking(int fd) {
    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0)) == -1) {
        log_err("fcntl error\n");
        exit(1);
    }
    flags &= ~O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        log_err("fcntl error\n");
        exit(1);
    }
    return 1;
}
