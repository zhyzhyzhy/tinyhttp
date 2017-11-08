//
// Created by 朱逸尘 on 2017/9/14.
//

#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "log.h"
#include "util.h"

#define LINE 1024

/**
 * read runtime config from the config file and save to the server_config struct
 * @param file_name the path of the config file
 * @param config server_config entity
 */
void read_config(char* file_name, server_config *config) {
    FILE* config_file = fopen(file_name, "r");
    if (config_file == NULL) {
        log_err("CANNOT LOAD CONFIG FILE");
        exit(1);
    }

    char item[LINE];
    memset(item, 0, LINE);
    while (fgets(item, LINE, config_file)) {
        trim(item);
        if (!strstr(item, "=")) {
            continue;
        }

        int i = 0;
        while (item[i] != '=') {
            i++;
        }
        item[i] = '\0';

        if (strcmp(item, "sub_reactor") == 0) {
            config->sub_reactor = atoi(item + i + 1);
        }
        else if(strcmp(item, "thread_num") == 0) {
            config->thread_num = atoi(item + i + 1);
        }
        else if(strcmp(item, "target_dir") == 0) {
            strcpy(config->target_dir, item + i + 1);
        }
        else if(strcmp(item, "index_file_name") == 0) {
            strcpy(config->index_file_name, item + i + 1);
        }
        else if(strcmp(item, "host") == 0) {
            strcpy(config->host, item + i + 1);
        }
        else if(strcmp(item, "port") == 0) {
            strcpy(config->port, item + i + 1);
        }
        memset(item, 0, LINE);
    }
}