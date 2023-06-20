#ifndef ERROR_H
#define ERROR_H

#include "./9cc.h"

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

#endif