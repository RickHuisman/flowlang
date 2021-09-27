#include <stddef.h>
#include <stdlib.h>
#include "node.h"

Identifier newIdent(const char *start, int length) {
  Identifier ident;
  ident.start = start;
  ident.length = length;
  return ident;
}

Node *newUnary(UnaryOperator op, Node *left) {
  Node *node = malloc(sizeof(Node));
  node->type = NODE_UNARY;
  node->as.unary.op = op;
  node->as.unary.expr = left;
  return node;
}

Node *newBinary(Node *left, BinaryOperator op, Node *right) {
  Node *node = malloc(sizeof(Node));
  node->type = NODE_BINARY;
  node->as.binary.left = left;
  node->as.binary.op = op;
  node->as.binary.right = right;
  return node;
}

Node *newLetAssign(Identifier ident, Node *expr) {
  Node *node = malloc(sizeof(Node));
  node->type = NODE_LET_ASSIGN;
  node->as.letAssign.ident = ident;
  node->as.letAssign.expr = expr;
  return node;
}

Node *newLetSet(Identifier ident, Node *expr) {
  Node *node = malloc(sizeof(Node));
  node->type = NODE_LET_SET;
  node->as.letSet.ident = ident;
  node->as.letSet.expr = expr;
  return node;
}

Node *newLetGet(Identifier ident) {
  Node *node = malloc(sizeof(Node));
  node->type = NODE_LET_GET;
  node->as.letGet.ident = ident;
  return node;
}

Node *newFunctionNode(Identifier name, Args *args, Node *body) {
  Node *node = malloc(sizeof(Node));
  node->type = NODE_FUNCTION;
  node->as.function.name = name;
  node->as.function.args = args;
  node->as.function.body = body;
  return node;
}

Node *newCall(Node *callee) {
  Node *node = malloc(sizeof(Node));
  node->type = NODE_CALL;
  node->as.call.callee = callee;
  return node;
}

Node *newBlock(ModuleAst *block) {
  Node *node = malloc(sizeof(Node));
  node->type = NODE_BLOCK;
  node->as.block.block = block;
  return node;
}

Node *newIfElse(Node *condition, Node *then, Node *else_) {
  Node *node = malloc(sizeof(Node));
  node->type = NODE_IF_ELSE;
  node->as.ifElse.condition = condition;
  node->as.ifElse.then = then;
  node->as.ifElse.else_ = else_;
  return node;
}

Node *newPrint(Node *expr) {
  Node *node = malloc(sizeof(Node));
  node->type = NODE_PRINT;
  node->as.print.expr = expr;
  return node;
}

Node *newNumber(double value) {
  Node *node = malloc(sizeof(Node));
  node->type = NODE_NUMBER;
  node->as.number = value;
  return node;
}
