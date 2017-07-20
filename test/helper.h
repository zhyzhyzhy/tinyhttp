//
// Created by zhy on 2/12/17.
//

#ifndef WEBSERVER_HELPER_H
#define WEBSERVER_HELPER_H

void server_error(int connfd, int status, char *error_message);
void response(int connfd);
void do_get(char* file_name, char* file_type);
void usage();


void sig_int(int sig);
#endif //WEBSERVER_HELPER_H
