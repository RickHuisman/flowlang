#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
  Token current;
  Token previous;
} Parser;

typedef enum {
  PREC_NONE,
  PREC_ASSIGNMENT,  // =
  PREC_OR,          // or
  PREC_AND,         // and
  PREC_EQUALITY,    // == !=
  PREC_COMPARISON,  // < > <= >=
  PREC_TERM,        // + -
  PREC_FACTOR,      // * /
  PREC_UNARY,       // ! -
  PREC_CALL,        // . ()
  PREC_PRIMARY,
} Precedence;

typedef Node (*ParseFn)();

typedef struct {
  ParseFn prefix;
  ParseFn infix;
  Precedence precedence;
} ParseRule;

Parser parser;

// TODO improve error handling?
static void error(const char *message) {
  printf("%s", message);
  exit(0);
}

static void advance() {
  parser.previous = parser.current;
for (;;) {
    parser.current = readToken();
    if (parser.current.type != TOKEN_ERROR) break;
    error("Unexpected EOF.");
  }
}

static bool check(TokenType type) {
  return parser.current.type == type;
}

static bool match(TokenType type) {
  if (!check(type)) return false;
  advance();
  return true;
}

static void consume(TokenType type, const char *message) {
  if (parser.current.type == type) {
    advance();
    return;
  }

  error(message);
}

static Node number() {
  double value = strtod(parser.previous.start, NULL);
  return newNumber(value);
}

static Node expression();
static ParseRule* getRule(TokenType type);
static Node parsePrecedence(Precedence precedence);

static Node unary() {
  TokenType op_token = parser.previous.type;
  UnaryOperator op;
  switch (op_token) {
  case TOKEN_MINUS: op = UNARY_NEGATE; break;
  case TOKEN_BANG: op = UNARY_NOT; break;
  default:
    error("Unkown unary operator.");
  }

  Node expr = expression();
  return newUnary(op, expr);
}

static Node binary(Node left) {
  TokenType op_token = parser.previous.type;
  BinaryOperator op;
  switch (op_token) { // TODO: Add more binary ops.
  case TOKEN_PLUS:          op = BINARY_ADD; break;
  case TOKEN_MINUS:         op = BINARY_SUBTRACT; break;
  case TOKEN_STAR:          op = BINARY_MULTIPLY; break;
  case TOKEN_SLASH:         op = BINARY_DIVIDE; break;
  default:
    error("Unkown binary operator.");
  }

  ParseRule *rule = getRule(op_token);
  Node right = parsePrecedence((Precedence)(rule->precedence + 1));

  return newBinary(left, op, right);
}

ParseRule rules[] = {
  // [TOKEN_LEFT_PAREN]    = {grouping, NULL,   PREC_NONE}, TODO Grouping
  [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
  [TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
  [TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SLASH]         = {NULL,     binary, PREC_FACTOR},
  [TOKEN_STAR]          = {NULL,     binary, PREC_FACTOR},
  [TOKEN_BANG]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_BANG_EQUAL]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQUAL_EQUAL]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_GREATER]       = {NULL,     NULL,   PREC_NONE},
  [TOKEN_GREATER_EQUAL] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LESS]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LESS_EQUAL]    = {NULL,     NULL,   PREC_NONE},
  // [TOKEN_IDENTIFIER]    = {NULL,     NULL,   PREC_NONE}, TODO
  [TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
  [TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
};

static Node parsePrecedence(Precedence precedence) {
  advance();
  ParseFn prefixRule = getRule(parser.previous.type)->prefix;
  if (prefixRule == NULL) {
    error("Expect expression.");
  }

  Node expr = prefixRule();

  while (precedence <= getRule(parser.current.type)->precedence) {
    advance();
    ParseFn infixRule = getRule(parser.previous.type)->infix;
    expr = infixRule(expr);
  }

  return expr;
}

static ParseRule *getRule(TokenType type) {
  return &rules[type];
}

static Node expression() {
  return parsePrecedence(PREC_ASSIGNMENT);
}

static Node expressionStatement() {
  Node expr = expression();
  consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
  return expr;
}

static Node declaration() {
  if (match(TOKEN_PRINT)) {
    return newPrint(expressionStatement());
  }

  /* if (match(TOKEN_LET)) { */
  /*   Identifier ident = parseIdent(); */
  /*   consume(TOKEN_EQUAL, "Expect '=' after identifier."); */

  /*   Node expr = expression(); */
  /*   consume(TOKEN_SEMICOLON, "Expect ';' after expression."); */

  /*   return newLetAssign(ident, expr); */
  /* } */
}

Node parse(const char *source) {
  initLexer(source);

  // Call `advance` to prime the parser.
  advance();

  Node head;
  Node *cur = &head;

  while (!match(TOKEN_EOF)) {
    Node decl = declaration();
    cur->next = &decl;
    cur = cur->next;
  }

  Node node;
  node.type = NODE_BLOCK;
  node.body = head.next;
  return node;
}
