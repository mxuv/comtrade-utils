#ifndef _STINGS_H_
#define _STINGS_H_

char upcase_letter(char c);
int stringlen(const char *str);
void stringcopy(char *dest, const char *src);
void stringcopy_c(char *dest, const char *src, int count);
void stringcopy_ec(char *dest, const char *src, char endchar);

#endif
