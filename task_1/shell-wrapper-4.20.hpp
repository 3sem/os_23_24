#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <assert.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_COM_SIZE 1024

struct comTok {

    size_t begin = -1, end = -1;

    comTok (size_t _begin = -1, size_t _end = -1) : begin (_begin), end (_end) {}
};

void run_cmd (char* cmd);

int parse_cmd (char* cmd, char** args, comTok* cmdTokens);