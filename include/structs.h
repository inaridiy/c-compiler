#ifndef STRUCTS_H
#define STRUCTS_H

typedef enum
{
    TK_RESERVED,
    TK_INDENT,
    TK_NUM,
    TK_EOF,
    TK_RETURN,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
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

typedef enum
{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_EQ,     // ==
    ND_NE,     // !=
    ND_LT,     // <
    ND_LE,     // <=
    ND_ASSIGN, // =
    ND_LVAR,
    ND_NUM,
    ND_RETURN,
    ND_IF,
    ND_ELSE,
    ND_WHILE,
    ND_FOR,

} NodeKind;

typedef struct Node Node;

struct Node
{
    NodeKind kind;
    Node *lhs;  // 左辺
    Node *rhs;  // 右辺
    int val;    // kindがND_NUMの時
    int offset; // kindがND_LVARの時
};

typedef struct LVar LVar;

struct LVar
{
    LVar *next;
    char *name;
    int len;
    int offset;
};

#endif