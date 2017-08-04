//
// Created by 朱逸尘 on 2017/8/4.
//

#include "util.h"
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
void non_blocking_write(int conn_fd, char *buffer, int length) {
    int has_write = 0;
    int once_write = 0;
    int left_write = length;
    while (left_write > 0) {
        once_write = (int) write(conn_fd, buffer + has_write, (size_t) left_write);
        if (once_write < 0 ) {
            if(errno != EINTR && errno != EWOULDBLOCK && errno != EAGAIN) {
                return;
            }
            continue;
        }
        has_write += once_write;
        if (length != has_write) {
            left_write -= once_write;
        }
        else {
            return;
        }
    }
}