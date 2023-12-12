cmake_minimum_required(VERSION 3.10)
 
project(integral)
 
set(SRC integral_main.c integral_funcs.c)

add_executable(integral ${SRC})
