cmake_minimum_required(VERSION 3.20)
project(flowlang C)

set(CMAKE_C_STANDARD 11)

add_executable(flowlang main.c lexer.c)
