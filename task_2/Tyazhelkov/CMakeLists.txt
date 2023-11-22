cmake_minimum_required(VERSION 3.15)

project(duplex_pipe)
set(CMAKE_CXX_STANDARD 20)

add_executable(main main.c)
add_library(pipe pipe.c)
target_link_libraries(main pipe)
