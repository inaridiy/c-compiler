#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "tokenize_utils.h"
#include "tokenizer.h"

Token *new_token(TokenKind kind, Token *cur, char *str, int len)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
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

        if (is_ident1(*p))
        {
            char *q = p;
            do
            {
                p++;
            } while (is_ident2(*p));
            cur = new_token(TK_INDENT, cur, q, p - q);
            continue;
        }

        error_at(p, "パースできません"); // TODO: これでいいのかわからん
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}