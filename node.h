#ifndef node_h
#define node_h

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

typedef struct Expr {
  NodeType type;
  union {
    // Number expression.
    double number;

    struct {
      UnaryOperator op;
      struct Expr *expr;
    } unary;

    struct {
      struct Expr *left;
      BinaryOperator op;
      struct Expr *right;
    } binary;

    struct {
      struct Expr *expr;
    } print;
  } as; // TODO: As?
} Node;

typedef struct Ast {
  Node *node;
  struct Ast *next;
} Ast;

//struct Node {
//  NodeType type;
//  struct Node *next;
//
//  // Unary expression.
//  UnaryOperator unaryOp;
//
//  // Unary, binary, print expression left value.
//  Node *left;
//
//  // Binary expression.
//  BinaryOperator binaryOp;
//  Node *right;
//
//  // Block expr.
//  Node *body;
//
//  // Number literal.
//  double number; // TODO: Make literal.
//};

Node *newUnary(UnaryOperator op, Node *left);

Node* newBinary(Node *left, BinaryOperator op, Node *right);

Node newBlock(Node body);

Node *newPrint(Node *expr);

Node *newNumber(double value);

#endif
