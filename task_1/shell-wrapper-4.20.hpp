#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <assert.h>
#include <sys/wait.h>

#define MAX_COM_SIZE 1024

void run_cmd (char* cmd);

void parse_cmd (char* cmd, char** args);