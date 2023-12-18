cmake_minimum_required(VERSION 3.0)
project(2-duplex-pipe C CXX)

include_directories(Inc/)

add_library(DuplexPipe Src/DuplexPipe.c)

add_executable(${PROJECT_NAME} Src/Main.c)
target_link_libraries(${PROJECT_NAME} PRIVATE DuplexPipe)

# Copy tests to build directory
configure_file(Tests/GenTestData.sh ${CMAKE_CURRENT_BINARY_DIR}/GenTestData.sh COPYONLY)
configure_file(Tests/CompareMd5.py ${CMAKE_CURRENT_BINARY_DIR}/CompareMd5.py COPYONLY)
configure_file(Tests/RunTests.sh ${CMAKE_CURRENT_BINARY_DIR}/RunTests.sh COPYONLY)