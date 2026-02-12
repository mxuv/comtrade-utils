#include "strutils.h"

int char2int(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return c - '7';
    if (c >= 'a' && c <= 'f')
        return c - 'W';

    return -1;
}
