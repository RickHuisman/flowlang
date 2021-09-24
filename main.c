#include "vm.h"

int main() {
  initVM();

  char* source = "let x = 10; print x + 5;";
//  char* source = "print 2; print 4;";
  interpret(source);

  return 0;
}
