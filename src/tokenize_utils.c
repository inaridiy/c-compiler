#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenize_utils.h"

bool strstart(char *p, char *q)
{
    int len = strlen(p);
    return memcmp(p, q, len) == 0;
}

bool is_ident1(char c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

bool is_ident2(char c)
{
    return is_ident1(c) || ('0' <= c && c <= '9');
}
