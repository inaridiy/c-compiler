#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"
#include "error.h"
#include "node_utils.h"
#include "tokenize_utils.h"
#include "global.h"

Token *new_token(TokenKind kind, Token *cur, char *str, int len)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

// インデントを消費する
Token *consume_indent()
{
    if (token->kind != TK_INDENT)
        return false;
    Token *old = token;
    token = token->next;
    return old;
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

bool consume_token(TokenKind kind)
{
    if (token->kind != kind)
        return false;
    token = token->next;
    return true;
}

bool expect(char *op, char *str)
{
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        error_at(token->str, str, op);
    token = token->next;
    return true;
}

int expect_number()
{
    if (token->kind != TK_NUM)
        error_at(token->str, "数値ではありません");
    int val = token->val;
    token = token->next;
    return val;
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

        if (strchr("+-*/()<>;={},", *p))
        {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (strchr("\'", *p))
        {
            p++;
            if (!isalpha(*p))
                continue;
            cur = new_token(TK_NUM, cur, p, 1);
            cur->val = *p;
            p++;
            if (strchr("\'", *p))
            {
                p++;
                continue;
            }
            else
                error("文字リテラルが閉じてねぇ？");
        }

        if (strstart("return", p) && !is_ident2(p[6]))
        {
            cur = new_token(TK_RETURN, cur, p, 6);
            p += 6;
            continue;
        }

        if (strstart("if", p) && !is_ident2(p[2]))
        {
            cur = new_token(TK_IF, cur, p, 2);
            p += 2;
            continue;
        }

        if (strstart("else", p) && !is_ident2(p[4]))
        {
            cur = new_token(TK_ELSE, cur, p, 4);
            p += 4;
            continue;
        }

        if (strstart("while", p) && !is_ident2(p[5]))
        {
            cur = new_token(TK_WHILE, cur, p, 5);
            p += 5;
            continue;
        }

        if (strstart("for", p) && !is_ident2(p[3]))
        {
            cur = new_token(TK_FOR, cur, p, 3);
            p += 3;
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