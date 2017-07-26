//
// Created by 朱逸尘 on 2017/7/25.
//

#ifndef TINYHTTP_LOG_H
#define TINYHTTP_LOG_H

#define RED     "\e[0;31m"
#define GREEN   "\e[0;32m"
#define YELLOW  "\e[0;33m"


#define log_warn(...) fprintf(stdout, YELLOW "[WARNING] " __VA_ARGS__)

#define log_err(...) fprintf(stderr, RED "[ERROR] " __VA_ARGS__)

#define log_info(...) fprintf(stdout, GREEN "[INFO] " __VA_ARGS__)

#define log(...) fprintf(stdout, __VA_ARGS__)

#endif //TINYHTTP_LOG_H
