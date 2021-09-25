#ifndef parser_h
#define parser_h

#include "lexer.h"
#include "node.h"

ModuleAst *parse(const char *source);

#endif
