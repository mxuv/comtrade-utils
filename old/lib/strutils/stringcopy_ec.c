#include "strutils.h"

void stringcopy_ec(char *dest, const char *src, char endchar)
{
    while (*src != endchar) {
        *dest = *src;
        dest++;
        src++;
    }

    *dest = 0;
}
