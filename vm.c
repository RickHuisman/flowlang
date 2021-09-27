#include <stdio.h>

#include "vm.h"
#include "node.h"
#include "object.h"
#include "compiler.h"
#include "debug.h"
#include "parser.h"

VM vm;

static void resetStack() {
  vm.stackTop = vm.stack;
  vm.frameCount = 0;
}

void initVM() {
  resetStack();

  initTable(&vm.globals);
  initTable(&vm.strings);
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

static bool call(ObjClosure *closure, int argCount) {
  // TODO:
//  if (argCount != function->arity) {
//    runtimeError("Expected %d arguments but got %d.",
//                 function->arity, argCount);
//    return false;
//  }

//  if (vm.frameCount == FRAMES_MAX) {
//    runtimeError("Stack overflow.");
//    return false;
//  }

  CallFrame *frame = &vm.frames[vm.frameCount++];
  frame->closure = closure;
  frame->ip = closure->function->chunk.code;
  frame->slots = vm.stackTop - argCount - 1;
  return true;
}

static bool callValue(Value callee, int argCount) {
  if (IS_OBJ(callee)) {
    switch (OBJ_TYPE(callee)) {
      case OBJ_CLOSURE:
        return call(AS_CLOSURE(callee), argCount);
      case OBJ_FUNCTION:
        return call(AS_FUNCTION(callee), argCount);
      default:
        break; // Non-callable object type.
    }
  }
  // TODO: Report error.
//  runtimeError("Can only call functions and classes.");
  return false;
}

static bool isFalseValue(Value value) {
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static InterpretResult run() {
  CallFrame *frame = &vm.frames[vm.frameCount - 1];

#define READ_BYTE() (*frame->ip++)

#define READ_SHORT() \
    (frame->ip += 2, \
    (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))

#define READ_CONSTANT() \
    (frame->closure->function->chunk.constants.values[READ_BYTE()])

#define READ_STRING() AS_STRING(READ_CONSTANT())

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
      case OP_ADD:
        BINARY_OP(NUMBER_VAL, +);
        break;
      case OP_SUBTRACT:
        BINARY_OP(NUMBER_VAL, -);
        break;
      case OP_DIVIDE:
        BINARY_OP(NUMBER_VAL, /);
        break;
      case OP_MULTIPLY:
        BINARY_OP(NUMBER_VAL, *);
        break;
      case OP_EQUAL: {
        Value b = pop();
        Value a = pop();
        push(BOOL_VAL(valuesEqual(a, b)));
        break;
      }
      case OP_GREATER:
        BINARY_OP(BOOL_VAL, >);
        break;
      case OP_LESS:
        BINARY_OP(BOOL_VAL, <);
        break;
      case OP_DEFINE_GLOBAL: {
        ObjString *name = READ_STRING();
        tableSet(&vm.globals, name, peek(0));
        pop();
        break;
      }
      case OP_GET_GLOBAL: {
        ObjString *name = READ_STRING();
        Value value;
        if (!tableGet(&vm.globals, name, &value)) {
          // TODO: Throw error.
//          runtimeError("Undefined variable '%s'.", name->chars);
          return INTERPRET_RUNTIME_ERROR;
        }
        push(value);
        break;
      }
      case OP_SET_GLOBAL: {
        ObjString *name = READ_STRING();
        if (tableSet(&vm.globals, name, peek(0))) {
          tableDelete(&vm.globals, name);
          // TODO: Throw error.
//          runtimeError("Undefined variable '%s'.", name->chars);
          return INTERPRET_RUNTIME_ERROR;
        }
        break;
      }
      case OP_GET_LOCAL: {
        uint8_t slot = READ_BYTE();
        push(frame->slots[slot]);
        break;
      }
      case OP_SET_LOCAL: {
        uint8_t slot = READ_BYTE();
        frame->slots[slot] = peek(0);
        break;
      }
      case OP_POP: {
        pop();
        break;
      }
      case OP_JUMP_IF_FALSE: {
        uint16_t offset = READ_SHORT();
        if (isFalseValue(peek(0))) frame->ip += offset;
        break;
      }
      case OP_JUMP: {
        uint16_t offset = READ_SHORT();
        frame->ip += offset;
        break;
      }
      case OP_CLOSURE: {
        ObjFunction *function = AS_FUNCTION(READ_CONSTANT());
        ObjClosure *closure = newClosure(function);
        push(OBJ_VAL(closure));
        break;
      }
      case OP_CALL: {
        int argCount = READ_BYTE();
        if (!callValue(peek(argCount), argCount)) {
          return INTERPRET_RUNTIME_ERROR;
        }
        frame = &vm.frames[vm.frameCount - 1];
        break;
      }
      case OP_PRINT: {
        printValue(pop());
        printf("\n");
        break;
      }
      case OP_RETURN: {
        Value result = pop();
        vm.frameCount--;
        if (vm.frameCount == 0) {
          pop();
          return INTERPRET_OK;
        }

        vm.stackTop = frame->slots;
        push(result);
        frame = &vm.frames[vm.frameCount - 1];
        break;
      }
    }
  }
#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_SHORT
#undef BINARY_OP
}

InterpretResult interpret(const char *source) {
  ModuleAst *ast = parse(source);

  ObjFunction *function = compile(ast);
  if (function == NULL) return INTERPRET_COMPILE_ERROR;

  push(OBJ_VAL(function));
  ObjClosure *closure = newClosure(function);
  pop();
  push(OBJ_VAL(closure));
  call(closure, 0);

  // TODO: Free chunk.

  return run();
}

