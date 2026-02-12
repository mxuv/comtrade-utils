#include <stdio.h>

#include "strutil.h"
#include "fileutil.h"

enum {max_token_count = 30};
enum {str_bufsize = 4096};

void print_sting_tokens(str_t *str)
{
    int i;

    printf("%d: %s\n", str->strnum+1, str->str);
    for (i = 0; i < str->token_count; i++) {
        printf("    token %.*s, start %d, length %d\n",
                str->tokens[i].tok_len,
                str->str + str->tokens[i].tok_start,
                str->tokens[i].tok_start,
                str->tokens[i].tok_len);
    }
}

int main(int argc, char **argv)
{
    FILE *fd;
    char strbuf[str_bufsize];
    struct token tks[max_token_count];
    str_t str;
    enum getstring_status gss;

    if (argc < 2) {
        fputs("Too few arguments\n", stderr);
        return 1;
    }

    fd = fopen(argv[1], "rb");
    if (fd == NULL) {
        perror(argv[1]);
        return 1;
    }

    str.tokens = &tks[0];
    str.str = strbuf;
    str.strnum = 0;
    while ((str.strlen = getstring(fd, strbuf, str_bufsize, &gss))) {
        if (gss)
            return 1;

        str.strlen = delete_crlf(str.str, str.strlen);
        str.token_count = parsing_tokens(str.str, str.strlen, ',',
                str.tokens);
        print_sting_tokens(&str);
        str.strnum++;
    }

    fclose(fd);
    return 0;
}
