//
// Created by zhy on 2/12/17.
//

#include <stdio.h>
#include <stdlib.h>

#define NON_NUM '0'

/**
 * printf the usage for the server
 */
void usage() {
    fprintf(stderr, "usage :: ./WebServer config_filename\n");
}

/**
 * judge whether the target char is 0-9 or a-z or A-Z
 * @param c the target char need to be judged
 * @return
 */
int hex2num(char c)
{
    if (c>='0' && c<='9') return c - '0';
    if (c>='a' && c<='z') return c - 'a' + 10;//这里+10的原因是:比如16进制的a值为10
    if (c>='A' && c<='Z') return c - 'A' + 10;
    return NON_NUM;
}

/**
 * parse the url encoded url to common
 * @param str
 * @param str_size
 */
void url_decode(const char* str, int str_size, char* result)
{
    char ch,ch1,ch2;
    int i;
    //record result index
    int j = 0;


    if ((str==NULL) || (result==NULL) || (str_size<=0)) {
        return;
    }

    for (i = 0; i < str_size; i++) {
        ch = str[i];
        switch (ch) {
            case '+':
                result[j++] = ' ';
                break;
            case '%':
                if (i+2<str_size) {
                    ch1 = (char) hex2num(str[i + 1]);//高4位
                    ch2 = (char) hex2num(str[i + 2]);//低4位
                    if ((ch1!=NON_NUM) && (ch2!=NON_NUM))
                        result[j++] = ((ch1<<4) | ch2);
                    i += 2;
                    break;
                } else {
                    break;
                }
            default:
                result[j++] = ch;
                break;
        }
    }
    result[j] = 0;
}