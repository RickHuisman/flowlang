#include "vm.h"

int main() {
  initVM();

  char* source = "let x = 10; x = 5; print x * 2;";
  interpret(source);

  return 0;
}
