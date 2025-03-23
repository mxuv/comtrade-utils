int stringlen(const char *str)
{
   const char *p;

    p = str;
    while (*p)
        p++;

    return p - str;
}

void stringcopy(char *dest, const char *src)
{
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }

    *dest = 0;
}

void stringcopy_ec(char *dest, const char *src, char endchar)
{
    while (*src != endchar) {
        *dest = *src;
        dest++;
        src++;
    }

    *dest = 0;
}

void stringcopy_c(char *dest, const char *src, int count)
{
    while (count) {
        *dest = *src;
        dest++;
        src++;
        count--;
    }

    *dest = 0;
}

int stringmatch(const char *str, const char *pattern)
{
    if (!*pattern)
        return *str == 0;
    if (*str != *pattern)
        return 0;
    else 
        return stringmatch(str + 1, pattern + 1);
}

char upcase_letter(char c)
{
    if (c >= 'a' && c <= 'z')
        return c - ('a' - 'A');
    else
        return c;
}

void upcase_string(char *str)
{
    while (*str) {
        *str = upcase_letter(*str);
        str++;
    }
}

