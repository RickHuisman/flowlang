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

typedef Node *(*ParseFn)();

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

static Node *number() {
  double value = strtod(parser.previous.start, NULL);
  return newNumber(value); // TODO
}

static Identifier parseIdentifier() {
  consume(TOKEN_IDENTIFIER, "Expect identifier.");
  Token source = parser.previous;
  return newIdent(source.start, source.length);
}

static Node *expression();

static Node *expressionStatement();

static Node *declaration();

static ParseRule *getRule(TokenType type);

static Node *parsePrecedence(Precedence precedence);

static Node *unary() {
  TokenType op_token = parser.previous.type;
  UnaryOperator op;
  switch (op_token) {
    case TOKEN_MINUS:
      op = UNARY_NEGATE;
      break;
    case TOKEN_BANG:
      op = UNARY_NOT;
      break;
    default:
      error("Unkown unary operator.");
  }

  return newUnary(op, expression());
}

static Node *binary(Node *left) {
  TokenType op_token = parser.previous.type;
  BinaryOperator op;
  switch (op_token) { // TODO: Add more binary ops.
    case TOKEN_PLUS:
      op = BINARY_ADD;
      break;
    case TOKEN_MINUS:
      op = BINARY_SUBTRACT;
      break;
    case TOKEN_STAR:
      op = BINARY_MULTIPLY;
      break;
    case TOKEN_SLASH:
      op = BINARY_DIVIDE;
      break;
    case TOKEN_BANG_EQUAL:
      op = BINARY_BANG_EQUAL;
      break;
    case TOKEN_EQUAL_EQUAL:
      op = BINARY_EQUAL;
      break;
    case TOKEN_GREATER:
      op = BINARY_GREATER;
      break;
    case TOKEN_GREATER_EQUAL:
      op = BINARY_GREATER_EQUAL;
      break;
    case TOKEN_LESS:
      op = BINARY_LESS;
      break;
    case TOKEN_LESS_EQUAL:
      op = BINARY_LESS_EQUAL;
      break;
    default:
      error("Unknown binary operator.");
  }

  ParseRule *rule = getRule(op_token);
  Node *right = parsePrecedence((Precedence) (rule->precedence + 1));

  return newBinary(left, op, right);
}

static Node *call(Node *left) {
  // TODO: Move into function.
  // TODO: Create a struct of (arglist, arity).
  int arity = 0;
  Node *args = malloc(MAX_ARG_COUNT*sizeof(Node));
  if (!check(TOKEN_RIGHT_PAREN)) {
    do {
      // TODO: Error.
//      if (current->function->arity > 255) {
//        errorAtCurrent("Can't have more than 255 parameters.");
//      }
      Node *foo = expression();
      args[arity] = *foo; // TODO: Dereference?
      arity += 1;
    } while (match(TOKEN_COMMA));
  }

  consume(TOKEN_RIGHT_PAREN, ""); // TODO: Message.

  return newCall(left, args, arity);
}

static Node *identifier() {
  Token source = parser.previous;
  Identifier ident = newIdent(source.start, source.length);

  if (match(TOKEN_EQUAL)) {
    return newLetSet(ident, expression());
  }

  return newLetGet(ident);
}

ParseRule rules[] = {
    [TOKEN_LEFT_PAREN]    = {NULL, call, PREC_CALL}, // TODO: Grouping.
    [TOKEN_RIGHT_PAREN]   = {NULL, NULL, PREC_NONE},
    [TOKEN_LEFT_BRACE]    = {NULL, NULL, PREC_NONE},
    [TOKEN_RIGHT_BRACE]   = {NULL, NULL, PREC_NONE},
    [TOKEN_COMMA]         = {NULL, NULL, PREC_NONE},
    [TOKEN_DOT]           = {NULL, NULL, PREC_NONE},
    [TOKEN_MINUS]         = {unary, binary, PREC_TERM},
    [TOKEN_PLUS]          = {NULL, binary, PREC_TERM},
    [TOKEN_SEMICOLON]     = {NULL, NULL, PREC_NONE},
    [TOKEN_SLASH]         = {NULL, binary, PREC_FACTOR},
    [TOKEN_STAR]          = {NULL, binary, PREC_FACTOR},
    [TOKEN_BANG]          = {NULL, NULL, PREC_NONE}, // TODO: Unary.
    [TOKEN_BANG_EQUAL]    = {NULL, binary, PREC_EQUALITY},
    [TOKEN_EQUAL]         = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL_EQUAL]   = {NULL, binary, PREC_EQUALITY},
    [TOKEN_GREATER]       = {NULL, binary, PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS]          = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS_EQUAL]    = {NULL, binary, PREC_COMPARISON},
    [TOKEN_IDENTIFIER]    = {identifier, NULL, PREC_NONE},
    [TOKEN_NUMBER]        = {number, NULL, PREC_NONE},
    [TOKEN_ERROR]         = {NULL, NULL, PREC_NONE},
    [TOKEN_EOF]           = {NULL, NULL, PREC_NONE},
};

static Node *parsePrecedence(Precedence precedence) {
  advance();
  ParseFn prefixRule = getRule(parser.previous.type)->prefix;
  if (prefixRule == NULL) {
    error("Expect expression.");
  }

  Node *expr = prefixRule();

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

static Node *expression() {
  return parsePrecedence(PREC_ASSIGNMENT);
}

static Node *expressionStatement() {
  Node *expr = expression();
  consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
  return expr;
}

static Node *parsePrint() {
  return newPrint(expressionStatement());
}

static Node *parseLetAssign() {
  Identifier ident = parseIdentifier();
  consume(TOKEN_EQUAL, "Expect '=' after identifier.");
  return newLetAssign(ident, expressionStatement());
}

static Node *parseBlock() {
  consume(TOKEN_LEFT_BRACE, "Expect '{'."); // TODO: Message.

  struct ModuleAst *block = NULL;
  while (!check(TOKEN_RIGHT_BRACE) &&
         !check(TOKEN_EOF)) {
    Node *decl = declaration();

    struct ModuleAst *link = malloc(sizeof(struct ModuleAst));
    link->node = decl;
    link->next = block;

    block = link;
  }

  consume(TOKEN_RIGHT_BRACE, "Expect '}' after parseBlock.");
  return newBlock(block);
}

static Node *parseIf() {
  Node *condition = expression();

  // Then clause.
  Node *then = parseBlock();

  // Else clause.
  Node *else_ = NULL;
  if (match(TOKEN_ELSE)) {
    else_ = parseBlock();
  }

  return newIfElse(condition, then, else_);
}

//static Identifier *parseArgs() {
////  static uint8_t argumentList() {
////    uint8_t argCount = 0;
////    if (!check(TOKEN_RIGHT_PAREN)) {
////      do {
////        expression();
////        argCount++;
////      } while (match(TOKEN_COMMA));
////    }
////    consume(TOKEN_RIGHT_PAREN, "Expect ')' after arguments.");
////    return argCount;
////  }
////
//
////  struct Args *args = NULL;
////  if (!check(TOKEN_RIGHT_PAREN)) {
////    do {
////      // TODO: Error.
//////      if (current->function->arity > 255) {
//////        errorAtCurrent("Can't have more than 255 parameters.");
//////      }
////      Identifier arg = parseIdentifier();
////
////      struct Args *link = malloc(sizeof(struct Args));
////      link->node = &arg; // TODO: as reference?
////      link->next = args;
////
////      args = link;
////
////      args->count++;
////    } while (match(TOKEN_COMMA));
////  }
//
//  return args;
//}

static Node *parseFunction() {
  Identifier name = parseIdentifier();

  consume(TOKEN_LEFT_PAREN, "Expect '(' after function name.");

  // TODO: Move into function.
  // TODO: Create a struct of (arglist, arity).
  int arity = 0;
  Identifier *args = malloc(MAX_ARG_COUNT*sizeof(Identifier));
  if (!check(TOKEN_RIGHT_PAREN)) {
    do {
      // TODO: Error.
//      if (current->function->arity > 255) {
//        errorAtCurrent("Can't have more than 255 parameters.");
//      }
      args[arity] = parseIdentifier();
      arity += 1;
    } while (match(TOKEN_COMMA));
  }

  consume(TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");

  Node *body = parseBlock();

  return newFunctionNode(name, args, arity, body);
}

static Node *declaration() {
  if (match(TOKEN_PRINT)) return parsePrint();
  if (match(TOKEN_LET)) return parseLetAssign();
  if (check(TOKEN_LEFT_BRACE)) return parseBlock();
  if (match(TOKEN_IF)) return parseIf();
  if (match(TOKEN_FUN)) return parseFunction();
  return expressionStatement();
}

ModuleAst *parse(const char *source) {
  initLexer(source);

  // Call `advance` to prime the parser.
  advance();

  // TODO: Cleanup.
  struct ModuleAst *ast = NULL;
  while (!match(TOKEN_EOF)) {
    Node *decl = declaration();

    struct ModuleAst *link = malloc(sizeof(struct ModuleAst));
    link->node = decl;
    link->next = ast;

    ast = link;
  }

  return ast;
}
