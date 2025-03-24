#ifndef _STINGS_H_
#define _STINGS_H_

int ishexdigit (char c);
int char2int(char c);
char upcase_letter(char c);
void upcase_string(char *str);
int stringlen(const char *str);
void stringcopy(char *dest, const char *src);
void stringcopy_c(char *dest, const char *src, int count);
void stringcopy_ec(char *dest, const char *src, char endchar);
int stringmatch(const char *str, const char *pattern);

#endif
