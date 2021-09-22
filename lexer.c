#include "lexer.h"
#include <string.h>
#include <stdbool.h>

typedef struct Lexer {
  const char *start;
  const char *current;
} Lexer;

Lexer lexer;

static bool isAlpha(char c) {
  return (c >= 'a' && c <= 'z') ||
    (c >= 'A' && c <= 'Z') ||
    c == '_';
}

static bool isDigit(char c) {
  return c >= '0' && c <= '9';
}

static bool isAtEnd() {
  return *lexer.current == '\0';
}

static char advance() {
  lexer.current++;
  return lexer.current[-1];
}

static char peekNext() {
  if (isAtEnd()) return '\0';
  return lexer.current[1];
}

static char peek() {
  return *lexer.current;
}

static void skipWhitespace() {
  for (;;) {
    char c = peek();
    switch (c) {
    case ' ':
    case '\r':
    case '\t':
    case '\n':
    advance();
    break;
    default:
    return;
    }
  }
}

static bool match(char expected) {
  if (isAtEnd()) return false;
  if (*lexer.current != expected) return false;

  lexer.current++;
  return true;
}

Token errorToken(const char* message) {
  Token token;
  token.type = TOKEN_ERROR;
  token.start = message;
  token.length = (int)strlen(message);
  return token;
}

Token makeToken(TokenType type) {
  Token token;
  token.type = type;
  token.start = lexer.start;
  token.length = (int)(lexer.current - lexer.start);
  return token;
}

static TokenType checkKeyword(int start, int length,
  const char* rest, TokenType type) {
  if (lexer.current - lexer.start == start + length &&
          memcmp(lexer.start + start, rest, length) == 0) {
      return type;
  }

  return TOKEN_IDENTIFIER;
}

static TokenType identifierType() {
  // TODO:
  switch (lexer.start[0]) {
  case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
  }

  return TOKEN_IDENTIFIER;
}

static Token identifier() {
  while (isAlpha(peek()) || isDigit(peek())) {
    advance();
  }

  return makeToken(identifierType());
}

static Token number() {
  while (isDigit(peek())) {
    advance();
  }

  // Look for a fractional part.
  if (peek() == '.' && isDigit(peekNext())) {
    // Consume the '.'.
    advance();

    while (isDigit(peek())) {
      advance();
    }
  }

  return makeToken(TOKEN_NUMBER);
}

Token readToken() {
  skipWhitespace();

  lexer.start = lexer.current;

  if (isAtEnd()) return makeToken(TOKEN_EOF);

  char c = advance();

  if (isAlpha(c)) return identifier();
  if (isDigit(c)) return number();

  switch (c) {
  case '(': return makeToken(TOKEN_LEFT_PAREN);
  case ')': return makeToken(TOKEN_RIGHT_PAREN);
  case '{': return makeToken(TOKEN_LEFT_BRACE);
  case '}': return makeToken(TOKEN_RIGHT_BRACE);
  case ';': return makeToken(TOKEN_SEMICOLON);
  case ',': return makeToken(TOKEN_COMMA);
  case '.': return makeToken(TOKEN_DOT);
  case '-': return makeToken(TOKEN_MINUS);
  case '+': return makeToken(TOKEN_PLUS);
  case '/': return makeToken(TOKEN_SLASH);
  case '*': return makeToken(TOKEN_STAR);
  case '!':
    return makeToken(
        match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
  case '=':
    return makeToken(
        match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
  case '<':
    return makeToken(
        match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
  case '>':
    return makeToken(
        match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
  }

  return errorToken("Unexpected character");
}

void initLexer(const char* source) {
  lexer.start = source;
  lexer.current = source;
}
