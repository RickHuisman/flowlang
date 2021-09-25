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

typedef struct {
  const char *start;
  int length;
} Identifier;

typedef enum {
  NODE_UNARY,
  NODE_BINARY,
  NODE_LET_ASSIGN,
  NODE_LET_SET,
  NODE_LET_GET,
  NODE_BLOCK,
  NODE_PRINT,
  NODE_NUMBER, // TODO: Change to NODE_LITERAL.
} NodeType;

typedef struct ModuleAst ModuleAst;

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
      Identifier ident;
      struct Expr *expr;
    } letAssign;

    struct {
      Identifier ident;
      struct Expr *expr;
    } letSet;

    struct {
      Identifier ident;
    } letGet;

    struct {
      ModuleAst *block;
    } block;

    struct {
      struct Expr *expr;
    } print;
  } as; // TODO: As?
} Node;

// TODO: Rename to Block?
typedef struct ModuleAst {
  Node *node;
  struct ModuleAst *next;
} ModuleAst;

Identifier newIdent(const char *start, int length);

Node *newUnary(UnaryOperator op, Node *left);

Node *newBinary(Node *left, BinaryOperator op, Node *right);

Node *newLetAssign(Identifier ident, Node *expr);

Node *newLetSet(Identifier ident, Node *expr);

Node *newLetGet(Identifier ident);

Node *newBlock(ModuleAst *ast);

Node *newPrint(Node *expr);

Node *newNumber(double value);

#endif
