#include <stddef.h>
#include <stdlib.h>
#include "node.h"

Node* newUnary(UnaryOperator op, Node *left) {
  Node *node = malloc(sizeof(Node));
  node->type = NODE_UNARY;
  node->as.unary.op = op;
  node->as.unary.expr = left;
  return node;
}

Node* newBinary(Node *left, BinaryOperator op, Node *right) {
  Node *node = malloc(sizeof(Node));
  node->type = NODE_BINARY;
  node->as.binary.left = left;
  node->as.binary.op = op;
  node->as.binary.right = right;
  return node;
}

//Node newBlock(Node body) {
//  Node node;
//  node.type = NODE_BLOCK;
//  node.body = body.next;
//  return node;
//}

Node* newPrint(Node *expr) {
  Node *node = malloc(sizeof(Node));
  node->type = NODE_PRINT;
  node->as.print.expr = expr;
  return node;
}

Node* newNumber(double value) {
  Node *node = malloc(sizeof(Node));
  node->type = NODE_NUMBER;
  node->as.number = value;
  return node;
}
