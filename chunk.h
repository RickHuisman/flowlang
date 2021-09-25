#ifndef chunk_h
#define chunk_h

#include <stdint.h>
#include "value.h"

typedef enum {
  OP_CONSTANT,
  OP_RETURN,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_EQUAL,
  OP_GREATER,
  OP_LESS,
  OP_NOT,
  OP_NEGATE,
  OP_DEFINE_GLOBAL,
  OP_GET_GLOBAL,
  OP_SET_GLOBAL,
  OP_GET_LOCAL,
  OP_SET_LOCAL,
  OP_POP,
  OP_JUMP,
  OP_JUMP_IF_FALSE,
  OP_PRINT,
} Opcode;

typedef struct {
  int count;
  int capacity;
  uint8_t *code;
  ValueArray constants;
} Chunk;

void initChunk(Chunk *chunk);
void freeChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, uint8_t byte);
int addConstant(Chunk *chunk, Value value);

#endif
