cmake_minimum_required(VERSION 3.0)
project(5-signal-file-transmission)

include_directories(Inc/)

add_library(Common Src/Common.c)
target_link_libraries(Common PUBLIC TnStatus pthread)

add_library(Queue Src/Queue.c)
target_link_libraries(Queue PUBLIC TnStatus)

add_library(Receiver Src/Receiver.c)
target_link_libraries(Receiver PUBLIC Common Queue)

add_library(Transmitter Src/Transmitter.c)
target_link_libraries(Transmitter PUBLIC Common)

add_library(RxDriver Src/RxDriver.c)
target_link_libraries(RxDriver PUBLIC Receiver)

add_library(TxDriver Src/TxDriver.c)
target_link_libraries(TxDriver PUBLIC Transmitter)

add_executable(RunTest Tests/RunTest.c)
target_link_libraries(RunTest PRIVATE RxDriver TxDriver)

add_executable(RxMain Src/RxMain.c)
target_link_libraries(RxMain PUBLIC RxDriver)

add_executable(TxMain Src/TxMain.c)
target_link_libraries(TxMain PUBLIC TxDriver)

# Copy scripts to build directory
configure_file(Tests/GenTestData.sh ${CMAKE_CURRENT_BINARY_DIR}/GenTestData.sh COPYONLY)
configure_file(Tests/GenTests.py ${CMAKE_CURRENT_BINARY_DIR}/GenTests.py COPYONLY)
configure_file(Tests/CompareMd5.py ${CMAKE_CURRENT_BINARY_DIR}/CompareMd5.py COPYONLY)