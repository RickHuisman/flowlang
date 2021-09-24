cmake_minimum_required(VERSION 3.20)
project(flowlang C)

set(CMAKE_C_STANDARD 11)

add_executable(flowlang main.c lexer.c node.c parser.c value.c chunk.c memory.c compiler.c vm.c object.c debug.c table.c table.h)
