#pragma once
#define _GNU_SOURCE

#include <signal.h>
#include <unistd.h>

#include "TnStatus.h"

#define CMD_STOP 0xFF
#define CMD_CONNECT 0xEE
#define CMD_FINISH 0xBB
#define CMD_START 0xAA

#define BASE_SIGNUM SIGRTMIN

#define DATA_INT_SIGNUM BASE_SIGNUM
#define DATA_CHAR_SIGNUM BASE_SIGNUM + 1
#define CMD_SIGNUM BASE_SIGNUM + 2

TnStatus SendSignal(pid_t pid, int sigNo, sigval_t value);
void ExitWithMessage(const char* msg);
void ExitWithErrno(const char* msg);
void SigactionWrapper(int sigNum, const struct sigaction* sigAction);