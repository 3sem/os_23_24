cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(task_6)
set(daemon_files src/daemon.cpp src/main.cpp include/daemon.h)
add_executable(daemon ${daemon_files})
