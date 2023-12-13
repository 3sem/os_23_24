cmake_minimum_required(VERSION 3.0)
project(1-shell-emulator C CXX)

include_directories(Inc/)

add_library(ShellStatus Src/ShellStatus.c)

add_library(TokenParser Src/TokenParser.c)
target_link_libraries(TokenParser PUBLIC ShellStatus)

add_library(Executor Src/Executor.c)
target_link_libraries(Executor PUBLIC ShellStatus)

add_library(Shell Src/Shell.c)
target_link_libraries(Shell PUBLIC TokenParser Executor)

add_executable(${PROJECT_NAME} Src/Main.c)
target_link_libraries(${PROJECT_NAME} PUBLIC Shell)

add_executable(RunTests Tests/RunTests.cc)
target_link_libraries(RunTests PRIVATE GTest::gtest_main Executor TokenParser)