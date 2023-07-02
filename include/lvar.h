#ifndef LVAR_H
#define LVAR_H

#include "structs.h"

LVar *find_lvar(Token *tok);
LVar *new_lvar(Token *tok);

#endif