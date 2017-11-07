//
// Created by 朱逸尘 on 2017/11/6.
//

#ifndef TINYHTTP_PARSER_H
#define TINYHTTP_PARSER_H

char* parse_http_code(int httpcode);
int parse_http_request_line(int conn_fd, char* method, char* path, char* version);
int check_read_done(char* request_head);

#endif //TINYHTTP_PARSER_H
