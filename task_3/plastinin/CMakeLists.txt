cmake_minimum_required(VERSION 3.20)

project(task_3)

set(task_files src/main.cpp src/shm.cpp src/fifo.cpp src/queue.cpp)
add_executable(task_3 ${task_files})
