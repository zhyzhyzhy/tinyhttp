//
// Created by 朱逸尘 on 2017/11/6.
//
#include "parser.h"
#include "mysocket.h"

/**
 * contains the http codes and the response
 */
http_status_pair http_status[] = {
        {200, "OK"},
        {404, "Not Found"},
        {405, "Method Not Allowed"},
        {500, "Internal Server Error"},
        {NULL, NULL}
};

/**
 *
 * @param httpcode the specific httpcode
 * @return the response of the httpcode
 */
char* parse_http_code(int httpcode) {
    http_status_pair pair;
    int i = 0;
    pair = http_status[i];
    while (pair.status_message != NULL) {
        if (pair.status_code == httpcode) {
            return pair.status_message;
        }
        pair = http_status[++i];
    }
    return NULL;
}

/**
 * check the request is done or not
 * @param request_head the message from once read operation
 * @return 1 the request is done and 0 the request is not done
 */
int check_read_done(char* request_head) {
    char* p = request_head;
    while (*p != '\0') {
        p++;
    }
    p -= 4;
    //if the last char is "\r\n\r\n" the request is done
    if (*p == '\r' && *(p+1) == '\n' && *(p+2) == '\r' && *(p+3) == '\n') {
        return 1;
    }
    return 0;
}

int parse_http_request_line(int conn_fd, char* method, char* path, char* version) {


    return 1;
}

