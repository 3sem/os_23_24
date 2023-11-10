#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include "flog.hpp" //a legacy logging + hash microlibrary by me

extern const char SHM_NAME[];
extern const size_t SHM_SIZE[];
extern const int UPPER_WAIT_LIMIT;

#define SMALL_FILE_COUNT "128"
#define SMALL_FILE_BS "128"
#define BIG_FILE_COUNT "5"
#define BIG_FILE_BS "1G"
#define TEST_FILE_NAME "test.rng"
#define OUTPUT_FILE_NAME "test_output.rng"

enum FlagValues {

    NUL = 0,
    READ = 1,
    WROTE = 2,
    EOF_MET = 3,
    EOF_WRITTEN = 4
};

size_t generateFile (int argc, char* argv[]);

char* createShm (size_t size);

void wait4Flag (char* flag, FlagValues waitTarget);