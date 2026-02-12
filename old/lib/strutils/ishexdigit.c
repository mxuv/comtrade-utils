#include "strutils.h"

int ishexdigit (char c)
{
    if ((c >= '0' && c <= '9') || 
        (c >= 'A' && c <= 'F') ||
        (c >= 'a' && c <= 'f'))
        return 1;
    else
        return 0;
}
