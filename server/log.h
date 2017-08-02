//
// Created by 朱逸尘 on 2017/7/25.
//

#ifndef TINYHTTP_LOG_H
#define TINYHTTP_LOG_H

#include <errno.h>
#include <string.h>

#define RED     "\e[0;31m"
#define GREEN   "\e[0;32m"
#define YELLOW  "\e[0;33m"


#define log_info(M, ...) fprintf(stdout, GREEN "[INFO]: " M "\n", ##__VA_ARGS__)

#define log_warn(M, ...) fprintf(stdout, YELLOW "[WARNING]: " M "\n", ##__VA_ARGS__)

#define log_err(M, ...) fprintf(stderr, RED "[ERROR]: " M "\n", ##__VA_ARGS__)

#define log_err_by_errno fprintf(stderr, RED "[ERROR]: %s\n", strerror(errno))

#define log(M, ...) fprintf(stdout, M "\n", ##__VA_ARGS__ )

#endif //TINYHTTP_LOG_H
