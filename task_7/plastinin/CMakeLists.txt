cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(task_7)
set(server_files src/server_main.cpp src/server.cpp include/server.h)
set(client_files src/client.cpp)

add_executable(server ${server_files})
add_executable(client ${client_files})
