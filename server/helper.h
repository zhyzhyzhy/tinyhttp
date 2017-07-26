//
// Created by zhy on 2/12/17.
//

#ifndef WEBSERVER_HELPER_H
#define WEBSERVER_HELPER_H

void usage();
int hex2num(char c);
int url_decode(const char* str, const int str_size, char* result, const int result_size);
#endif //WEBSERVER_HELPER_H
