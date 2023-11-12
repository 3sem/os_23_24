cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(task_5)
set(snd_files src/sigsnd.cpp)
add_executable(snd ${snd_files})

set(rcv_files src/sigrcv.cpp)
add_executable(rcv ${rcv_files})
