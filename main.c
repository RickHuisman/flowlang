#include "vm.h"

int main() {
  initVM();

  char* source = "print 12 / 6;";
  interpret(source);

  return 0;
}
