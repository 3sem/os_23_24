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
extern const long long UPPER_WAIT_LIMIT;

#define SMALL_FILE_COUNT "512"
#define SMALL_FILE_BS "128"
#define BIG_FILE_COUNT "5"
#define BIG_FILE_BS "1G"
#define TEST_FILE_NAME "test.rng"
#define OUTPUT_FILE_NAME "test_output.rng"

enum FlagValues {

    FL_NULL = 0,
    FL_READ = 1 << 0,
    FL_WROTE = 1 << 1,
    FL_EOF = 1 << 2,
    FL_EOF_CONFIRMED = 1 << 3,
    FL_NO_WAIT_LIMIT = 1 << 4,
    FL_SERVER_READY = 1 << 5,
    FL_CLIENT_READY = 1 << 6,
    FL_ERROR = 1 << 7
};

size_t generateFile (int argc, char* argv[]);

char* createShm (size_t size);

void wait4Flag (char* flag, FlagValues waitTarget);

void flogFlag (char* flag);