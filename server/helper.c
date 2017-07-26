//
// Created by zhy on 2/12/17.
//

#include <stdio.h>

#define NON_NUM '0'

void usage() {
    fprintf(stderr, "usage :: WebServer host port index_home\n");
}

int hex2num(char c)
{
    if (c>='0' && c<='9') return c - '0';
    if (c>='a' && c<='z') return c - 'a' + 10;//这里+10的原因是:比如16进制的a值为10
    if (c>='A' && c<='Z') return c - 'A' + 10;

    printf("unexpected char: %c", c);
    return NON_NUM;
}
/**
 * @brief URLDecode 对字符串URL解码,编码的逆过程
 *
 * @param str 原字符串
 * @param str_size 原字符串大小（不包括最后的\0）
 * @param result 结果字符串缓存区
 * @param result_size 结果地址的缓冲区大小(包括最后的\0)
 *
 * @return: >0 result 里实际有效的字符串长度
 *            0 解码失败
 *
 *
 */
int url_decode(const char* str, const int str_size, char* result, const int result_size)
{
    char ch,ch1,ch2;
    int i;
    int j = 0;//record result index

    if ((str==NULL) || (result==NULL) || (str_size<=0) || (result_size<=0)) {
        return 0;
    }

    for ( i=0; (i<str_size) && (j<result_size); ++i) {
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
                        result[j++] = (char)((ch1<<4) | ch2);
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
    return j;
}