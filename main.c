#include <stdio.h>
#include "lexer.h"

int main() {
  char* source = "print 10;";

  initLexer(source);

  Token token = readToken();
  printf("%2d '%.*s'\n", token.type, token.length, token.start);

  return 0;
}
