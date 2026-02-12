#ifndef _STRUTILS_H_
#define _STRUTILS_H_

#include <stdio.h>

enum getstring_status {
    gss_ok,
    gss_eof,
    gss_err,
    gss_overflow
};

int ishexdigit (char c);
int char2int(char c);
int match_char(char ch, char patt);
char upcase_letter(char c);
void upcase_string(char *str);
int stringlen(const char *str);
void stringcopy(char *dest, const char *src);
void stringcopy_c(char *dest, const char *src, int count);
void stringcopy_ec(char *dest, const char *src, char endchar);
int stringmatch(const char *str, const char *pattern);
int getstring(FILE *fd, char *buffer, int bufsize,
                enum getstring_status *status);

#endif
