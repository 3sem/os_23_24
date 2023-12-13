#pragma once
#include "Transmitter.h"

typedef struct {
  Transmitter Transmitter;
  pid_t RxPid;
} TransmitterContext;

// Global transmitter context
// There is a way to manage it without using global variable.
// That way utilizes linux's ucontext and it is way better.
// But I didn't manage to utilize it, so the straightforward
// solution (global variable for context) is used.
static TransmitterContext TxCtx;

static void TxSigHandler(int sigNum, siginfo_t* sigInfo, void*);

void TxDriver(pid_t rxPid);