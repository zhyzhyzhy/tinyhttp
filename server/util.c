//
// Created by 朱逸尘 on 2017/8/4.
//

#include <string.h>
#include <unistd.h>
#include <errno.h>

/**
 * write buffer to target fd
 * @param conn_fd the fd that write to
 * @param buffer  char buffer
 * @param length  the length of buffer
 */
void non_blocking_write(int conn_fd, const char *buffer, int length) {
    int has_write = 0;
    int once_write = 0;
    int left_write = length;
    while (left_write > 0) {
        once_write = (int) write(conn_fd, buffer + has_write, (size_t) left_write);
        if (once_write < 0) {
            if (errno != EINTR && errno != EWOULDBLOCK && errno != EAGAIN) {
                return;
            }
            continue;
        }
        has_write += once_write;
        if (length != has_write) {
            left_write -= once_write;
        } else {
            return;
        }
    }
}

/**
 * remove char' ' in the head and tail of the target str
 * @param str the target str
 */
void trim(char *str) {
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] == ' ') {
            int j = i + 1;
            while (str[j] != '\0') {
                str[j - 1] = str[j];
                j++;
            }
        } else if (str[i] == '\n') {
            str[i] = '\0';
        } else {
            i++;
        }
    }
}

/**
 * judge if the target str is end with ch
 *
 * @param str the target str
 * @param ch  the check char
 * @return 1 is false, 0 is true
 */
int is_end_with_ch(char* str, char ch) {
    int length = (int)strlen(str);
    if (str[length-1] == ch) {
        return 1;
    }
    return 0;
}