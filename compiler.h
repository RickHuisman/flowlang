#ifndef compiler_h
#define compiler_h

#include "vm.h"
#include "node.h"
#include "object.h"

ObjFunction *compile(Ast *ast);

#endif
