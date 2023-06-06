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

        if (strchr("+-*/()<>", *p))
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

        error("トークナイズできねぇ！！！");
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}