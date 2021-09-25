#include "vm.h"

int main() {
  initVM();

  char* source = "if 5 < 2 { print 5; } else { print 2; }";

//  char* source = "print 5 > 2;";
  interpret(source);

  return 0;
}
