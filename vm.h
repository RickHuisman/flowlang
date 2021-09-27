#ifndef vm_h
#define vm_h

#include "chunk.h"
#include "value.h"
#include "table.h"
#include "object.h"

// TODO: Move into common.h?
#define UINT8_COUNT (UINT8_MAX + 1)

#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

typedef struct {
  ObjClosure *closure;
  uint8_t *ip;
  Value *slots;
} CallFrame;

typedef struct {
  CallFrame frames[FRAMES_MAX];
  int frameCount;

  Value stack[STACK_MAX];
  Value *stackTop;
  Table globals;
  Table strings;
} VM;

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR,
} InterpretResult;

extern VM vm;

void initVM();

InterpretResult interpret(const char *source);

void push(Value value);

Value pop();

#endif
