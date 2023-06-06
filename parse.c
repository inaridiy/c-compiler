#include "9cc.h"

Token *token;
char *user_input;

Token *new_token(TokenKind kind, Token *cur, char *str, int len)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

bool consume(char *op)
{
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        return false;
    token = token->next;
    return true;
}

Token *consume_indent()
{
    if (token->kind != TK_INDENT)
        return false;
    Token *old = token;
    token = token->next;
    return old;
}

void expect(char *op)
{
    if (token->kind != TK_RESERVED || strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        error("'%c'ではありません", op);
    token = token->next;
}

int expect_number()
{
    if (token->kind != TK_NUM)
        error("数ではありません");
    int val = token->val;
    token = token->next;
    return val;
}

bool strstart(char *p, char *q)
{
    int len = strlen(p);
    return memcmp(p, q, len) == 0;
}

bool at_eof()
{
    return token->kind == TK_EOF;
}

Token *tokenize(char *p)
{
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p)
    {
        if (isspace(*p))
        {
            p++;
            continue;
        }

        if (strstart("==", p) || strstart("!=", p) || strstart("<=", p) || strstart(">=", p))
        {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (strchr("+-*/()<>;=", *p))
        {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (isdigit(*p))
        {
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        if (*p == '\'')
        {
            p++;
            if (isalpha(*p))
            {
                cur = new_token(TK_NUM, cur, p, 1);
                cur->val = *p;
                p++;
                if (*p == '\'')
                {
                    p++;
                    continue;
                }
                else
                {
                    error("文字リテラルが閉じてねぇ？");
                }
            }
        }

        if ('a' <= *p && *p <= 'z')
        {
            cur = new_token(TK_INDENT, cur, p++, 1);
            continue;
        }

        error("トークナイズできねぇ！！！");
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}

Node *node;
Node *code[100];

Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;

    return node;
}

Node *new_node_num(int val)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;

    return node;
}

Node *primary()
{
    Token *tok = consume_indent();
    if (tok)
    {
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;
        node->offset = (tok->str[0] - 'a' + 1) * 8;
        return node;
    }

    if (consume("("))
    {
        Node *node = expr();
        expect(")");
        return node;
    }

    return new_node_num(expect_number());
}

Node *unary()
{
    if (consume("+"))
        return primary();
    if (consume("-"))
        return new_node(ND_SUB, new_node_num(0), primary());
    return primary();
}

Node *mul()
{
    Node *node = unary();

    for (;;)
    {
        if (consume("*"))
            node = new_node(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

Node *add()
{
    Node *node = mul();

    for (;;)
    {
        if (consume("+"))
            node = new_node(ND_ADD, node, mul());
        else if (consume("-"))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

Node *relational()
{
    Node *node = add();

    for (;;)
    {
        if (consume("<"))
            node = new_node(ND_LT, node, add());
        else if (consume(">"))
            node = new_node(ND_LT, add(), node);
        else if (consume("<="))
            node = new_node(ND_LE, node, add());
        else if (consume(">="))
            node = new_node(ND_LE, add(), node);
        else
            return node;
    }
}

Node *equality()
{
    Node *node = relational();

    for (;;)
    {
        if (consume("=="))
            return node = new_node(ND_EQ, node, relational());
        else if (consume("!="))
            return node = new_node(ND_NE, node, relational());
        else
            return node;
    }
}

Node *assign()
{
    Node *node = equality();
    if (consume("="))
        node = new_node(ND_ASSIGN, node, assign());
    return node;
}

Node *expr()
{
    return assign();
}

Node *stmt()
{
    Node *node = expr();
    expect(";");
    return node;
}

void program()
{
    int i = 0;
    while (!at_eof())
        code[i++] = stmt();
    code[i] = NULL;
}