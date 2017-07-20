/**
	File Name: echo.c
	Author: zhy
	Created Time: 2017/02/12 - 15:00:06
*/
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>


#define BACKLOG 1024
#define BUFFERSIZE 8192

int main(int argc, char *argv[])
{

	if (argc != 2) {
		printf("usage: Server port\n");
		exit(1);
	}

	int listenfd;
	int connfd;

	struct sockaddr_in addr_server;
	int sockaddr_len = sizeof(struct sockaddr);

	//get port
	unsigned short port;
	char *temp;
	port = strtol(argv[1], &temp, 0);
	

	memset(&addr_server, 0, sizeof(struct sockaddr_in));

	addr_server.sin_family      = AF_INET;
    inet_aton("127.0.0.1", &addr_server.sin_addr);
    addr_server.sin_port        = htons(port);

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd == -1) {
		perror("can't listen : ");
		exit(1);
	}

	if(bind(listenfd, (struct sockaddr*)&addr_server, sizeof(struct sockaddr_in)) == -1) {
		perror("can't bind : ");
		exit(1);
	}

	char buffer[BUFFERSIZE];
	while(1) {
		close(connfd);
		listen(listenfd, BACKLOG);
		connfd = accept(listenfd, (struct sockaddr*)&addr_server, &sockaddr_len);
		printf("get connect\n");
		int size = read(connfd, buffer, 8192);
		send(connfd, buffer, size, 0);
	}
	return 0;
}
