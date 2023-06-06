#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    int len; // トークンの長さ
};

Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool consume(char *op);
Token *consume_indent();
void expect(char *op);
int expect_number();
bool strstart(char *p, char *q);
bool at_eof();
Token *tokenize(char *p);

void error(char *fmt, ...);

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

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *primary();
Node *unary();
Node *mul();
Node *add();
Node *relational();
Node *equality();
Node *assign();
Node *expr();
Node *stmt();
void program();

void gen(Node *node);

int main(int argc, char **argv);

extern Token *token;
extern char *user_input;
extern Node *node;
extern Node *code[100];