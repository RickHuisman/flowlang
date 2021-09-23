#ifndef parser_h
#define parser_h

#include "lexer.h"
#include "node.h"

Ast *parse(const char *source);

#endif
