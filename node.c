#include "node.h"

Node newUnary(UnaryOperator op, Node left) {
  Node node;
  node.type = NODE_UNARY;
  node.unaryOp = op;
  node.left = &left;
  return node;
}

Node newBinary(Node left, BinaryOperator op, Node right) {
  Node node;
  node.type = NODE_BINARY;
  node.left = &left;
  node.binaryOp = op;
  node.right = &right;
  return node;
}

Node newPrint(Node left) {
  Node node;
  node.type = NODE_PRINT;
  node.left = &left;
  return node;
}

Node newNumber(double value) {
  Node node;
  node.type = NODE_NUMBER;
  node.number = value;
  return node;
}
