#ifndef object_h
#define object_h

#include "chunk.h"

typedef enum {
  OBJ_FUNCTION,
} ObjType;

struct Obj {
  ObjType type;
  struct Obj *next;
};

typedef struct {
  Obj obj;
  int arity;
  Chunk chunk;
  // TODO: Name.
} ObjFunction;

ObjFunction *newFunction();

#endif
