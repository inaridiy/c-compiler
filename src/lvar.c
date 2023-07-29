#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "lvar.h"

LVar *locals;

// ローカル変数を検索する
LVar *find_lvar(Token *tok)
{
    for (LVar *var = locals; var; var = var->next)
        if (var->len == tok->len && memcmp(tok->str, var->name, var->len) == 0)
            return var;
    return NULL;
}

LVar *new_lvar(Token *tok)
{
    if (find_lvar(tok))
        error_at(tok->str, "変数の再定義です");

    LVar *lvar;
    lvar = calloc(1, sizeof(LVar));
    lvar->name = tok->str;
    lvar->len = tok->len;
    if (locals == NULL)
    {
        lvar->next = NULL;
        lvar->offset = 8;
        locals = lvar;
        return lvar;
    }
    else
    {
        lvar->next = locals;
        lvar->offset = locals->offset + 8;
        locals = lvar;
        return lvar;
    }
}