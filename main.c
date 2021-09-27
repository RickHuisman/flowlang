#include "vm.h"

int main() {
  initVM();

//  char* source = "if 5 < 2 { print 5; } else { print 2; }";
  char* source = "fun foo(x, y) { print 10 + 5; } foo();";

//  char* source = "print 5 > 2;";
  InterpretResult result = interpret(source);

  return 0;
}
