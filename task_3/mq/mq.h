#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>
#include <time.h>
#include <mqueue.h>
#include <assert.h>
#include <err.h>
#include <errno.h>
#include <error.h>
// #include "flog.hpp" //a legacy logging + hash microlibrary by me

//Externs for vars in shm_lib.cpp
extern const char SERVER_Q_NAME_TEMPLATE[];
extern const char CLIENT_Q_NAME_TEMPLATE[];
extern const int MAX_MSG_CNT;
extern const int CAP;
extern const size_t MSG_SIZE[];
extern const long long UPPER_WAIT_LIMIT;
extern const mode_t Q_PERM;
extern const int NAME_LEN_CAP;

// File generation related defines + output name
#define SMALL_FILE_COUNT "512"
#define SMALL_FILE_BS "128"
#define BIG_FILE_COUNT "5"
#define BIG_FILE_BS "1G"
#define TEST_FILE_NAME "test.rng"
#define OUTPUT_FILE_NAME "test_output.rng"

size_t generateFile (int argc, char* argv[]);

void asrt (bool clause, const char* msg, const char* q1, const char* q2);