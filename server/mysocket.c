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
#include <unistd.h>
//内核监听队列的最大长度
#define BACKLOG 1024

struct sockaddr_in get_conn_info(int conn_fd) {
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    getpeername(conn_fd, (struct sockaddr *) &client, &len);
    return client;
}

int socket_bind_listen(char *addr, char *port) {
    int listen_fd;
    unsigned short Port;
    struct sockaddr_in addr_s;
    //AF_INET ipv4
    //SOCK_STREAM 以流的方式传递
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        log_err("%s", strerror(errno));
        exit(1);
    }

    //get port
    Port = (unsigned short) atoi(port);

    memset(&addr_s, 0, sizeof(addr_s));
    addr_s.sin_family = AF_INET;
    addr_s.sin_addr.s_addr = inet_addr(addr);
    addr_s.sin_port = htons(Port);

    int ret = bind(listen_fd, (struct sockaddr *) &addr_s, sizeof(addr_s));
    if (ret == -1) {
        log_err("%s", strerror(errno));
        close(listen_fd);
        exit(1);
    }

    if (listen(listen_fd, BACKLOG) == -1) {
        log_err("listen error");
        close(listen_fd);
        exit(1);
    }
    return listen_fd;
}

int set_no_blocking(int fd) {
    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0)) == -1) {
        log_err("%s", strerror(errno));
        return 0;
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        log_err("%s", strerror(errno));
        return 0;
    }
    return 1;
}

int set_blocking(int fd) {
    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0)) == -1) {
        log_err_by_errno;
        return 0;
    }
    flags &= ~O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        log_err_by_errno;
        return 0;
    }
    return 1;
}
