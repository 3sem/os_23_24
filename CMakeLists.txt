cmake_minimum_required(VERSION 3.0)
project(os-sem3-hw C CXX)

# C compile flags
set(CMAKE_C_FLAGS "-Werror -g -O0")
set(CMAKE_CXX_FLAGS "-Werror -g -O0")

# Dependencies
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        main
)

FetchContent_Declare(
  TnStatus
  GIT_REPOSITORY https://github.com/tndrd/TnStatus.git
  GIT_TAG        master
)

FetchContent_Declare(
  TnAsync
  GIT_REPOSITORY https://github.com/tndrd/TnAsync.git
  GIT_TAG        master
)

FetchContent_Declare(
  TnCppHelpers
  GIT_REPOSITORY https://github.com/tndrd/TnCppHelpers.git
  GIT_TAG        master
)

FetchContent_MakeAvailable(googletest TnStatus TnAsync TnCppHelpers)

# Projects
add_subdirectory(1-shell-emulator)
add_subdirectory(2-duplex-pipe)
add_subdirectory(3-ipc-comparison)
add_subdirectory(4-monte-carlo)
add_subdirectory(5-signal-file-transmission)
add_subdirectory(6-backup-daemon)