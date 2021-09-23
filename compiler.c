#include <stdio.h>

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

static void initCompiler(Compiler *compiler, FunctionType type) {
  compiler->enclosing = current;
  compiler->function = NULL; // TODO: Necessary?
  compiler->type = type;
  compiler->function = newFunction();
  current = compiler;
}

static ObjFunction *endCompiler() {
  emitByte(OP_RETURN); // TODO:

  ObjFunction *function = current->function;
  current = current->enclosing;
  return function;
}

static void compileBlock(Node *node) {
//  printf("BLOCK");
//  for (Node *n = node->body; n; n = n->next) {
//    compile(n);
//  }

  // TODO: Fix.
//  compile(node->body);
//  Node *next = node->body;
//  while (next != NULL) {
//    printf("foo\n");
//    next = NULL;
//  }
}

static void compileNode(Node *node); // TODO: Placement.

static void compileUnary(Node *node) {
  compileNode(node->as.unary.expr);
  switch(node->as.unary.op) {
  case UNARY_NEGATE: emitByte(OP_NEGATE); break;
  /* case UNARY_NOT: emitByte(OP_NOT); break; */ // TODO:
  }
}

static void compileBinary(Node *node) {
  compileNode(node->as.binary.left);
  compileNode(node->as.binary.right);

  switch(node->as.binary.op) {
  case BINARY_ADD: emitByte(OP_ADD); break;
  case BINARY_SUBTRACT: emitByte(OP_SUBTRACT); break;
  case BINARY_MULTIPLY: emitByte(OP_MULTIPLY); break;
  case BINARY_DIVIDE: emitByte(OP_DIVIDE); break;
  }
}

static void compilePrint(Node *node) {
  compileNode(node->as.print.expr);
  emitByte(OP_PRINT);
}

static void compileNumber(Node *node) {
  emitConstant(NUMBER_VAL(node->as.number));
}

static void compileNode(Node *node) {
  switch (node->type) {
    case NODE_BLOCK: {
      break;
      case NODE_UNARY:
        compileUnary(node);
        break;
      case NODE_BINARY:
        compileBinary(node);
      break;
      case NODE_PRINT:
        compilePrint(node);
      break;
      case NODE_NUMBER:
        compileNumber(node);
      break;
    }
  }
}

ObjFunction *compile(Ast *ast) {
  Compiler compiler;
  initCompiler(&compiler, TYPE_SCRIPT);

  // Compile ast.
  for (Ast *cur = ast; cur; cur = cur->next) {
    compileNode(cur->node);
  }

  return endCompiler();
}

