//
// Created by 朱逸尘 on 2017/9/14.
//

#ifndef TINYHTTP_CONFIG_H
#define TINYHTTP_CONFIG_H

/**
 * sub_reactor         the number of the sub_reactor
 * thread_num          the number of the threads in threadpool
 * target_dir          where the static file in
 * index_file_name     if request is a dir, the index_file_name is the default of the dir
 * host                the running host
 * port                the running port
 */
struct server_config {
    int sub_reactor;
    int thread_num;
    char target_dir[1024];
    char index_file_name[1024];
    char host[24];
    char port[24];
};
typedef struct server_config server_config;

void read_config(char *file_name, server_config *config);

#endif //TINYHTTP_CONFIG_H
