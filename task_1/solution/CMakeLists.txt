cmake_minimum_required(VERSION 3.10)
 
project(terminal)
 
set(SRC terminal_main.c terminal_funcs.c)
 
add_executable(terminal ${SRC})
