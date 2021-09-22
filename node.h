#ifndef node_h
#define node_h

typedef struct Node Node;

typedef enum {
  UNARY_NEGATE,
  UNARY_NOT,
} UnaryOperator;

typedef enum {
  BINARY_ADD,
  BINARY_SUBTRACT,
  BINARY_MULTIPLY,
  BINARY_DIVIDE,
  BINARY_EQUAL,
  BINARY_BANG_EQUAL,
  BINARY_GREATER_EQUAL,
  BINARY_GREATER,
  BINARY_LESS_EQUAL,
  BINARY_LESS,
} BinaryOperator;

typedef enum {
  NODE_UNARY,
  NODE_BINARY,
  NODE_BLOCK,
  NODE_PRINT,
  NODE_NUMBER, // TODO: Change to NODE_LITERAL.
} NodeType;

struct Node {
  NodeType type;
  Node *next;

  // Unary expression.
  UnaryOperator unaryOp;

  // Unary, binary, print expression left value.
  Node *left;

  // Binary expression.
  BinaryOperator binaryOp;
  Node *right;

  // Block expr.
  Node *body;

  // Number literal.
  double number; // TODO: Make literal.
};

#endif
