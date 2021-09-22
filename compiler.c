#include <stdio.h>
#include <stdbool.h> // TODO: Remove.

#include "compiler.h"
#include "object.h"



typedef enum FunctionType {
  TYPE_FUNCTION,
  TYPE_METHOD,
  TYPE_SCRIPT,
} FunctionType;

typedef struct Compiler {
  struct Compiler *enclosing;
  ObjFunction *function;
  FunctionType type;
  // TODO: Locals.
  // TODO: scopeDepth.
} Compiler;

Compiler *current = NULL;

static Chunk *currentChunk() {
  return &current->function->chunk;
}

static void emitByte(uint8_t byte) {
  writeChunk(currentChunk(), byte);
}

static void emitBytes(uint8_t byte1, uint8_t byte2) {
  emitByte(byte1);
  emitByte(byte2);
}

static uint8_t makeConstant(Value value) {
  int constant = addConstant(currentChunk(), value);
  if (constant > UINT8_MAX) {
    // error("Too many constants in one chunk."); TODO:
    return 0;
  }

  return (uint8_t) constant;
}

static void emitConstant(Value value) {
  emitBytes(OP_CONSTANT, makeConstant(value));
}

static void initCompiler(Compiler* compiler, FunctionType type) {
  compiler->enclosing = current;
  compiler->function = NULL; // TODO: Necessary?
  compiler->type = type;
  compiler->function = newFunction();
  current = compiler;
}

static ObjFunction *endCompiler() {
  // TODO: emit return.
  ObjFunction *function = current->function;
  current = current->enclosing;
  return function;
}

static void compileBlock(Node *node) {
  printf("BLOCK");
  for (Node *n = node->body; n; n = n->next) {
    compile(n);
  }

  // TODO: Fix.
//  compile(node->body);
//  Node *next = node->body;
//  while (next != NULL) {
//    printf("foo\n");
//    next = NULL;
//  }
}

static void compileNumber(Node *node) {
  emitConstant(NUMBER_VAL(node->number));
}

static void compileNode(Node *node); // TODO: Placement.

static void compilePrint(Node *node) {
  compileNode(node->left);
  emitByte(OP_PRINT);
}

static void compileNode(Node *node) {
  switch (node->type) {
    case NODE_BLOCK: {
      // TODO:
//      Node *next = node->body;
//      while (next != NULL) {
//        printf("foobar\n");
//        compileNode(next);
//        next = next->next;
//      }
    }
//      printf("BLOCK\n");
//      for (Node *n = node->body; n; n = n->next) {
//          compile(n);
//      }
      break;
    case NODE_UNARY:
      break;
    case NODE_BINARY:
      break;
    case NODE_PRINT:
      compilePrint(node);
      break;
    case NODE_NUMBER:
      compileNumber(node);
      break;
  }
}

ObjFunction *compile(Node *ast) {
  Compiler compiler;
  initCompiler(&compiler, TYPE_SCRIPT);

  // Compile ast node.
  compileNode(ast);

  return endCompiler();
}
