#include <stdio.h>

#include "vm.h"
#include "node.h"
#include "object.h"
#include "compiler.h"
#include "debug.h"

VM vm;

static void resetStack() {
  vm.stackTop = vm.stack;
}

void initVM() {
  resetStack();
}

void push(Value value) {
  *vm.stackTop = value;
  vm.stackTop++;
}

Value pop() {
  vm.stackTop--;
  return *vm.stackTop;
}

static Value peek(int distance) {
  return vm.stackTop[-1 - distance];
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

#define BINARY_OP(valueType, op)                      \
  do {                                                \
    if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
      return INTERPRET_RUNTIME_ERROR;                 \
    }                                                 \
    double b = AS_NUMBER(pop());                      \
    double a = AS_NUMBER(pop());                      \
    push(valueType(a op b));                          \
  } while (false)

  for (;;) {
    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
      case OP_CONSTANT: {
      Value constant = READ_CONSTANT();
      push(constant);
      break;
    }
    case OP_ADD: {
      BINARY_OP(NUMBER_VAL, +);
    }
    case OP_PRINT: {
      printValue(pop());
      printf("\n");
    }
    case OP_RETURN: {
      return INTERPRET_OK;
    }
    }
    printf("Hello, World!\n");
  }
#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

InterpretResult interpret(const char *source) {
  Node ast = newPrint(newNumber(10.0));
  ObjFunction *function = compile(&ast);

  vm.chunk = &function->chunk;
  vm.ip = vm.chunk->code;

  disassembleChunk(vm.chunk, "TODO");

  InterpretResult result = run();

  // TODO: Free chunk.

  return result;
}

