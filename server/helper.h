//
// Created by zhy on 2/12/17.
//

#ifndef WEBSERVER_HELPER_H
#define WEBSERVER_HELPER_H

void usage();
int hex2num(char c);
void url_decode(const char* str, int str_size, char* result);
#endif //WEBSERVER_HELPER_H
