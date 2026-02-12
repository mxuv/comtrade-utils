#ifndef __STRUTIL_H__
#define __STRUTIL_H__

struct token {
    int tok_start;
    int tok_len;
};

typedef struct {
    char *str;
    struct token *tokens;
    int strlen;
    int strnum;
    int token_count;
} str_t;


int parsing_tokens(const char *str, int len, char delim, struct token *tks);
int delete_crlf(char *str, int strlen);

#endif
