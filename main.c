#include "vm.h"

int main() {
  initVM();

  char* source = "fun foo(x, y) { print x + y; } foo(5, 2);";

  interpret(source);

  return 0;
}
