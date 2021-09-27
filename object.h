#ifndef object_h
#define object_h

#include "chunk.h"
#include "value.h"

#define OBJ_TYPE(value)        (AS_OBJ(value)->type)

#define IS_CLOSURE(value)      isObjType(value, OBJ_CLOSURE)
#define IS_FUNCTION(value)     isObjType(value, OBJ_FUNCTION)

#define AS_CLOSURE(value)      ((ObjClosure*)AS_OBJ(value))
#define AS_FUNCTION(value)     ((ObjFunction*)AS_OBJ(value))
#define AS_STRING(value)       ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value)      (((ObjString*)AS_OBJ(value))->chars)

typedef enum {
  OBJ_CLOSURE,
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
  ObjString *name;
} ObjFunction;

typedef struct {
  Obj obj;
  ObjFunction *function;
} ObjClosure;

struct ObjString {
  Obj obj;
  int length;
  char *chars;
  uint32_t hash;
};

ObjClosure *newClosure(ObjFunction *function);

ObjFunction *newFunction();

ObjString *copyString(const char *chars, int length);

void printObject(Value value);

#endif
