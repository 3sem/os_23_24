cmake_minimum_required(VERSION 3.10)
 
project(duplex)
 
set(SRC duplex_main.c duplex_funcs.c)
 
add_executable(duplex ${SRC})
