//
// Created by 朱逸尘 on 2017/11/6.
//

#ifndef TINYHTTP_PARSER_H
#define TINYHTTP_PARSER_H

typedef struct {
    int status_code;
    const char *status_message;
}http_status_pair;

typedef struct {
    const char* type;
    const char* value;
}http_mime_type;

const char* parse_http_code(int httpcode);
int parse_http_request_line(int conn_fd, char* method, char* path, char* version);
int check_read_done(char* request_head);
const char* parse_mime_type(const char* path);

#endif //TINYHTTP_PARSER_H
