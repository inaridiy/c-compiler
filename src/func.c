#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "func.h"

Func *funcs;

Func *find_func(Token *tok)
{
    char *special1 = "foo";
    if (tok->len == 3 && memcmp(tok->str, special1, 3) == 0)
    {
        Func *func = calloc(1, sizeof(Func));
        func->name = special1;
        func->len = 3;
        return func;
    }

    char *special2 = "bar";

    if (tok->len == 3 && memcmp(tok->str, special2, 3) == 0)
    {
        Func *func = calloc(1, sizeof(Func));
        func->name = special2;
        func->len = 3;
        return func;
    }

    char *special3 = "printDigit";

    if (tok->len == 10 && memcmp(tok->str, special3, 10) == 0)
    {
        Func *func = calloc(1, sizeof(Func));
        func->name = special3;
        func->len = 10;
        return func;
    }

    for (Func *func = funcs; func; func = func->next)
        if (func->len == tok->len && memcmp(tok->str, func->name, func->len) == 0)
            return func;

    return NULL;
}

Func *new_func(Token *tok)
{
    Func *func;
    func = calloc(1, sizeof(Func));
    func->name = tok->str;
    func->len = tok->len;
    if (funcs == NULL)
    {
        func->next = NULL;
        funcs = func;
        return func;
    }
    else
    {
        func->next = funcs;
        funcs = func;
        return func;
    }
}