#ifndef table_h
#define table_h

#include <stdint.h>
#include "value.h"

typedef struct {
  ObjString *key;
  Value value;
} Entry;

typedef struct {
  int count;
  int capacity;
  Entry *entries;
} Table;

void initTable(Table *table);

bool tableGet(Table *table, ObjString *key, Value *value);

bool tableSet(Table *table, ObjString *key, Value value);

ObjString *tableFindString(Table *table, const char *chars,
                           int length, uint32_t hash);

#endif