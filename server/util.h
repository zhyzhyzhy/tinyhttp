//
// Created by 朱逸尘 on 2017/8/4.
//

#ifndef TINYHTTP_UTIL_H
#define TINYHTTP_UTIL_H

void non_blocking_write(int conn_fd, char *buffer, int length);
void trim(char* str);
int is_end_with_ch(char* str, char ch);
#endif //TINYHTTP_UTIL_H
