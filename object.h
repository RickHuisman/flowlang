#ifndef object_h
#define object_h

#include "chunk.h"
#include "value.h"

#define AS_STRING(value)       ((ObjString*)AS_OBJ(value))

typedef enum {
  OBJ_FUNCTION,
  OBJ_STRING,
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

struct ObjString {
  Obj obj;
  int length;
  char* chars;
  uint32_t hash;
};

ObjFunction *newFunction();

ObjString *copyString(const char *chars, int length);

#endif
