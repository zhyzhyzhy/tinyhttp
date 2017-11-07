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

#ifdef DEBUG
#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define debug(M, ...)
#endif


#define log_info(M, ...) fprintf(stdout, GREEN "%s:%d: [INFO]: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define log_warn(M, ...) fprintf(stdout, YELLOW "%s:%d: [WARNING]: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define log_err(M, ...) fprintf(stderr, RED "%s:%d: [ERROR]: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define log_err_by_errno fprintf(stderr, RED "%s:%d: [ERROR]: %s\n", __FILE__, __LINE__, strerror(errno))

#define log(M, ...) fprintf(stdout, M " \n", ##__VA_ARGS__ )

#endif //TINYHTTP_LOG_H
