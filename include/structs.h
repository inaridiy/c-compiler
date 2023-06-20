#ifndef STRUCTS_H
#define STRUCTS_H

typedef enum
{
    TK_RESERVED,
    TK_INDENT,
    TK_NUM,
    TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token
{
    TokenKind kind;
    Token *next;
    int val;
    char *str;
    int len;
};

#endif // STRUCTS_H