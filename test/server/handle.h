//
// Created by zhy on 2/21/17.
//

#ifndef WEBSERVER_HANDLE_H
#define WEBSERVER_HANDLE_H
void do_get(int connfd, char* file_name, char* file_type);
void response(int connfd);
void server_error(int connfd, int status);
char* parse_http_code(int status);
#endif //WEBSERVER_HANDLE_H
