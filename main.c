#include <stdio.h>

#include "lexer.h"
#include "parser.h"
#include "compiler.h"

int main() {
  char* source = "print 10;";

  Node ast = newPrint(newNumber(10.0));
  ObjFunction *function = compile(&ast);
  printf("foo");

//  Node ast = parse(source);
//  compile(&ast);

//  initLexer(source);
//
//  Token token = readToken();
//  if (token.type == TOKEN_PRINT) {
//    printf("FOOBAR");
//  }
//  printf("%2d '%.*s'\n", token.type, token.length, token.start);

  return 0;
}
