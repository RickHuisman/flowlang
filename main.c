#include "vm.h"

int main() {
  initVM();

  char* source = "{ let x = 5; x = 10; print x; }";
  interpret(source);

  return 0;
}
