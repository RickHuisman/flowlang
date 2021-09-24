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
  OP_NEGATE,
  OP_GET_GLOBAL,
  OP_DEFINE_GLOBAL,
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
