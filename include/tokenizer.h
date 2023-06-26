#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdbool.h>
#include "structs.h"

Token *tokenize(char *p);

Token *consume_indent();
bool consume(char *op);
bool expect(char *op, char *str);
int expect_number();
bool at_eof();

#endif