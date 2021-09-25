#include <stdio.h>

#include "compiler.h"
#include "object.h"

// TODO: Move into "common.h"
#define UINT8_COUNT (UINT8_MAX + 1)

typedef struct {
  Identifier name;
  int depth;
  bool isCaptured;
} Local;

typedef enum FunctionType {
  TYPE_FUNCTION,
  TYPE_METHOD,
  TYPE_SCRIPT,
} FunctionType;

typedef struct Compiler {
  struct Compiler *enclosing;
  ObjFunction *function;
  FunctionType type;
  Local locals[UINT8_MAX];
  int localCount;
  int scopeDepth;
  // TODO: Locals.
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

static int emitJump(uint8_t instruction) {
  emitByte(instruction);
  emitByte(0xff);
  emitByte(0xff);
  return currentChunk()->count - 2;
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

static void patchJump(int offset) {
  // -2 to adjust for the bytecode for the jump offset itself.
  int jump = currentChunk()->count - offset - 2;

  if (jump > UINT16_MAX) {
    // TODO: Throw error.
//    error("Too much code to jump over.");
  }

  currentChunk()->code[offset] = (jump >> 8) & 0xff;
  currentChunk()->code[offset + 1] = jump & 0xff;
}

static void initCompiler(Compiler *compiler, FunctionType type) {
  compiler->enclosing = current;
  compiler->function = NULL; // TODO: Necessary?
  compiler->type = type;
  compiler->localCount = 0;
  compiler->scopeDepth = 0;
  compiler->function = newFunction();
  current = compiler;

  // TODO:
//  Local* local = &current->locals[current->localCount++];
//  local->depth = 0;
////> Closures init-zero-local-is-captured
//  local->isCaptured = false;
}

static ObjFunction *endCompiler() {
  emitByte(OP_RETURN); // TODO:

  ObjFunction *function = current->function;
  current = current->enclosing;
  return function;
}

static void beginScope() {
  current->scopeDepth++;
}

static void endScope() {
  current->scopeDepth--;

  while (current->localCount > 0 &&
         current->locals[current->localCount - 1].depth >
         current->scopeDepth) {
    emitByte(OP_POP);
    current->localCount--;
  }
}

static uint8_t identifierConstant(Identifier *name) {
  return makeConstant(OBJ_VAL(copyString(name->start,
                                         name->length)));
}

static bool identifiersEqual(Identifier *a, Identifier *b) {
  if (a->length != b->length) return false;
  return memcmp(a->start, b->start, a->length) == 0;
}

static int resolveLocal(Compiler *compiler, Identifier *name) {
  for (int i = compiler->localCount - 1; i >= 0; i--) {
    Local *local = &compiler->locals[i];
    if (identifiersEqual(name, &local->name)) {
      if (local->depth == -1) {
        // TODO: Throw error.
//        error("Can't read local variable in its own initializer.");
      }
      return i;
    }
  }

  return -1;
}

static void compileNode(Node *node); // TODO: Placement.

static void compileAst(ModuleAst *ast);

static void compileUnary(Node *node) {
  compileNode(node->as.unary.expr);
  switch (node->as.unary.op) {
    case UNARY_NEGATE:
      emitByte(OP_NEGATE);
      break;
      /* case UNARY_NOT: emitByte(OP_NOT); break; */ // TODO:
  }
}

static void compileBinary(Node *node) {
  compileNode(node->as.binary.left);
  compileNode(node->as.binary.right);

  switch (node->as.binary.op) {
    case BINARY_ADD:
      emitByte(OP_ADD);
      break;
    case BINARY_SUBTRACT:
      emitByte(OP_SUBTRACT);
      break;
    case BINARY_MULTIPLY:
      emitByte(OP_MULTIPLY);
      break;
    case BINARY_DIVIDE:
      emitByte(OP_DIVIDE);
      break;
    case BINARY_EQUAL:
      emitByte(OP_EQUAL);
      break;
    case BINARY_BANG_EQUAL:
      emitBytes(OP_EQUAL, OP_NOT);
      break;
    case BINARY_GREATER_EQUAL:
      emitBytes(OP_LESS, OP_NOT);
      break;
    case BINARY_GREATER:
      emitByte(OP_GREATER);
      break;
    case BINARY_LESS_EQUAL:
      emitBytes(OP_GREATER, OP_NOT);
      break;
    case BINARY_LESS:
      emitByte(OP_LESS);
      break;
  }
}

static void addLocal(Identifier name) {
  if (current->localCount == UINT8_COUNT) {
//    error("Too many local variables in function.");
    // TODO: Throw error.
    return;
  }

  Local *local = &current->locals[current->localCount++];
  local->name = name;
  local->depth = -1;
  local->isCaptured = false;
}

static void declareVariable(Identifier *name) { // TODO: Pass as reference?
  if (current->scopeDepth == 0) return;

  for (int i = current->localCount - 1; i >= 0; i--) {
    Local *local = &current->locals[i];
    if (local->depth != -1 && local->depth < current->scopeDepth) {
      break;
    }

    if (identifiersEqual(name, &local->name)) {
//      error("Already variable with this name in this scope."); // TODO: Throw error.
    }
  }

  addLocal(*name); // TODO: Dereference?
}

static void markInitialized() {
  if (current->scopeDepth == 0) return;
  current->locals[current->localCount - 1].depth =
      current->scopeDepth;
}

// TODO: Pass as reference?
static void defineVariable(Identifier *name) {
  if (current->scopeDepth > 0) {
    markInitialized();
    return;
  }

  uint8_t global = identifierConstant(name);
  emitBytes(OP_DEFINE_GLOBAL, global);
}

static void compileLetAssign(Node *node) {
  declareVariable(&node->as.letAssign.ident);

  // Compile initializer.
  compileNode(node->as.letAssign.expr);

  defineVariable(&node->as.letAssign.ident);
}

static void compileLetGet(Node *node) {
  int arg = resolveLocal(current, &node->as.letGet.ident);
  if (arg != -1) {
    // Local variable.
    emitBytes(OP_GET_LOCAL, (uint8_t) arg);
  } else {
    // Global variable.
    uint8_t constant = identifierConstant(&node->as.letGet.ident);
    emitBytes(OP_GET_GLOBAL, constant);
  }
}

static void compileLetSet(Node *node) {
  compileNode(node->as.letSet.expr);

  int arg = resolveLocal(current, &node->as.letSet.ident);
  if (arg != -1) {
    // Local variable.
    emitBytes(OP_SET_LOCAL, (uint8_t) arg);
  } else {
    // Global variable.
    uint8_t constant = identifierConstant(&node->as.letSet.ident);
    emitBytes(OP_SET_GLOBAL, constant);
  }
}

static void compileBlock(Node *node) {
  beginScope();
  compileAst(node->as.block.block);
  endScope();
}

static void compileIfElse(Node *node) {
  compileNode(node->as.ifElse.condition);

  // Jump to else branch if false.
  int thenJump = emitJump(OP_JUMP_IF_FALSE);
  emitByte(OP_POP);

  compileBlock(node->as.ifElse.then);

  int elseJump = emitJump(OP_JUMP);

  patchJump(thenJump);
  emitByte(OP_POP);

  // Compile else branch if set.
  Node *elseBranch = node->as.ifElse.else_;
  if (elseBranch != NULL) {
    compileBlock(elseBranch);
  }

  patchJump(elseJump);
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
    case NODE_UNARY:
      compileUnary(node);
      break;
    case NODE_BINARY:
      compileBinary(node);
      break;
    case NODE_LET_ASSIGN:
      compileLetAssign(node);
      break;
    case NODE_LET_SET:
      compileLetSet(node);
      break;
    case NODE_LET_GET:
      compileLetGet(node);
      break;
    case NODE_BLOCK:
      compileBlock(node);
      break;
    case NODE_IF_ELSE:
      compileIfElse(node);
      break;
    case NODE_PRINT:
      compilePrint(node);
      break;
    case NODE_NUMBER:
      compileNumber(node);
      break;
  }
}

// TODO: Cleanup. Find a cleaner way to traverse the parseBlock.
static void compileAst(ModuleAst *ast) {
  if (ast->next != NULL) {
    compileAst(ast->next);
  }

  compileNode(ast->node);
}

ObjFunction *compile(ModuleAst *ast) {
  Compiler compiler;
  initCompiler(&compiler, TYPE_SCRIPT);

  compileAst(ast);

  return endCompiler();
}

