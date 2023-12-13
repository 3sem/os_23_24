cmake_minimum_required(VERSION 3.0)
project(6-backup-daemon CXX)

include_directories(Inc/)

add_library(WDCache Src/WDCache.cpp)
target_link_libraries(WDCache PUBLIC TnException)

add_library(FSMonitor Src/FSMonitor.cpp)
target_link_libraries(FSMonitor PUBLIC WDCache TnFileDescriptor TnLogger TnSelector TnFiles)

add_library(PathTree Src/PathTree.cpp)
target_link_libraries(PathTree PUBLIC TnException TnFiles)

add_library(IEventObserver Src/IEventObserver.cpp)

add_library(Incremental Src/Incremental.cpp)
target_link_libraries(Incremental PUBLIC IEventObserver TnException TnFiles)

add_library(BackupProducer Src/BackupProducer.cpp)
target_link_libraries(BackupProducer PUBLIC PathTree IEventObserver)

add_library(FifoChannel Src/FifoChannel.cpp)
target_link_libraries(FifoChannel PUBLIC TnPThread TnPipe TnFileDescriptor TnSelector)

add_library(BackupService Src/BackupService.cpp)
target_link_libraries(BackupService PUBLIC FSMonitor BackupProducer FifoChannel Incremental)

add_executable(BackupTest Test/BackupTest.cpp)
target_link_libraries(BackupTest PRIVATE BackupService)

add_executable(FSMTest Test/FSMTest.cpp)
target_link_libraries(FSMTest PUBLIC FSMonitor BackupProducer Incremental)

add_executable(SetPeriod Exec/SetPeriod.cpp)
target_link_libraries(SetPeriod PRIVATE FifoChannel)

add_executable(StartService Exec/StartService.cpp)
target_link_libraries(StartService PRIVATE BackupService)

# Copy scripts to build directory
configure_file(Scripts/Restore.py ${CMAKE_CURRENT_BINARY_DIR}/Restore.py COPYONLY)