cmake_minimum_required(VERSION 3.0)
project(3-ipc-comparison C CXX)

include_directories(Inc/)

# IPC Primitives

add_library(Helpers Src/IPC/Helpers.c)
add_library(IPCStatus Src/IPC/IPCStatus.c)

add_library(FIFOTx Src/IPC/FIFOTx.c)
target_link_libraries(FIFOTx PRIVATE Helpers)

add_library(FIFORx Src/IPC/FIFORx.c)
target_link_libraries(FIFORx PRIVATE Helpers)

add_library(ShMemSync Src/IPC/ShMemSync.c)
target_link_libraries(ShMemSync PRIVATE pthread)

add_library(ShMemTx Src/IPC/ShMemTx.c)
target_link_libraries(ShMemTx PRIVATE Helpers)

add_library(ShMemRx Src/IPC/ShMemRx.c)
target_link_libraries(ShMemRx PRIVATE Helpers)

add_library(MQTx Src/IPC/MQTx.c)
target_link_libraries(MQTx PRIVATE Helpers)

add_library(MQRx Src/IPC/MQRx.c)
target_link_libraries(MQRx PRIVATE Helpers)

# Drivers

add_library(BaseDriver Src/Drivers/BaseDriver.cpp)
target_link_libraries(BaseDriver PRIVATE IPCStatus)

add_library(FifoDriver Src/Drivers/FifoDriver.cpp)
target_link_libraries(FifoDriver PRIVATE FIFOTx FIFORx BaseDriver)

add_library(ShmDriver Src/Drivers/ShmDriver.cpp)
target_link_libraries(ShmDriver PRIVATE ShMemTx ShMemRx BaseDriver ShMemSync)

add_library(MsgDriver Src/Drivers/MsgDriver.cpp)
target_link_libraries(MsgDriver PRIVATE MQTx MQRx BaseDriver)

# Executables

add_executable(Fifo Src/Executables/Fifo.cpp)
target_link_libraries(Fifo PRIVATE FifoDriver)

add_executable(Shm Src/Executables/Shm.cpp)
target_link_libraries(Shm PRIVATE ShmDriver)

add_executable(Msg Src/Executables/Msg.cpp)
target_link_libraries(Msg PRIVATE MsgDriver)

# Benchmarks

add_executable(FifoBenchmark Src/Executables/FifoBenchmark.cpp)
target_link_libraries(FifoBenchmark PRIVATE FifoDriver)

add_executable(ShmBenchmark Src/Executables/ShmBenchmark.cpp)
target_link_libraries(ShmBenchmark PRIVATE ShmDriver)

add_executable(MsgBenchmark Src/Executables/MsgBenchmark.cpp)
target_link_libraries(MsgBenchmark PRIVATE MsgDriver)

# Copy tests to build directory
configure_file(Tests/GenTestData.sh ${CMAKE_CURRENT_BINARY_DIR}/GenTestData.sh COPYONLY)
configure_file(Tests/CompareMd5.py ${CMAKE_CURRENT_BINARY_DIR}/CompareMd5.py COPYONLY)
configure_file(Tests/GenTests.py ${CMAKE_CURRENT_BINARY_DIR}/GenTests.py COPYONLY)

# Copy benchmarks to build directory
configure_file(Scripts/GenBenchmarkData.sh ${CMAKE_CURRENT_BINARY_DIR}/GenBenchmarkData.sh COPYONLY)
configure_file(Scripts/RunBenchmarks.sh ${CMAKE_CURRENT_BINARY_DIR}/RunBenchmarks.sh COPYONLY)
configure_file(Scripts/Plot.py ${CMAKE_CURRENT_BINARY_DIR}/Plot.py COPYONLY)