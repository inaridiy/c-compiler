#ifndef GLOBAL_H
#define GLOBAL_H

#include "structs.h"

extern Token *token;
extern char *user_input;
extern Node *code[100];

void error(char *fmt, ...);

#endif