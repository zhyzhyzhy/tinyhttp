/**
	File Name: main.c
	Author: zhy
	Created Time: 2017/02/11 - 13:12:22
*/

#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include "helper.h"

//内核监听队列的最大长度
#define BACKLOG 1024


int main(int argc, char *argv[])
{
    signal(SIGINT, sig_int);
	if (argc != 3) {
        usage();
		exit(1);
	}

	int server_fd, conn_fd;
	unsigned short port;
	struct sockaddr_in addr_s;
    struct sockaddr_in addr_c;
    //AF_INET ipv4
	//SOCK_STREAM 以流的方式传递
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		perror("socket :");
		exit(1);
	}

    //get port
    port = (unsigned short)atoi(argv[2]);

    memset(&addr_s, 0, sizeof(addr_c));
	addr_s.sin_family = AF_INET;
    addr_s.sin_addr.s_addr = inet_addr(argv[1]);
    addr_s.sin_port = htons(port);

	int ret = bind(server_fd, (struct sockaddr*)&addr_s, sizeof(addr_c));
	if(ret == -1) {
		perror("bind :");
		exit(1);
	}


	int addr_clen = sizeof(addr_c);
    listen(server_fd, BACKLOG);
	while(1) {
		conn_fd = accept(server_fd, (struct sockaddr*)&addr_c, (socklen_t *) &addr_clen);
		if(fork() == 0) {
			close(server_fd);
            response(conn_fd);
			return 0;
		}
		else {
			close(conn_fd);
		}
	}
	return 0;
}
