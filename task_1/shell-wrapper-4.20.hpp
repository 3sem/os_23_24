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
#define MAX_CMD_CNT 32
#define CMD_SEQ_SIZE 32

struct Command {

    char* seq[CMD_SEQ_SIZE];
    char first = 0;
    char last = 0;

    Command (char** const seq_ = NULL, char first_ = 0, char last_ = 0) : first (first_), last (last_) {

        if (seq_ == NULL) return;
        int i = 0;
        for (i = 0; seq_[i] != NULL; i++) seq[i] = seq_[i];
        seq[i] = NULL;
    }
};

void run_cmd (char* cmd);

int parse_cmd (char* cmd, Command* output);