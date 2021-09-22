#include <stdio.h>

#include "lexer.h"
#include "parser.h"

int main() {
  char* source = "print 10;";

  Node ast = parse(source);

//  initLexer(source);
//
//  Token token = readToken();
//  if (token.type == TOKEN_PRINT) {
//    printf("FOOBAR");
//  }
//  printf("%2d '%.*s'\n", token.type, token.length, token.start);

  return 0;
}
