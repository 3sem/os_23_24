#pragma once
#include "Receiver.h"

typedef struct {
  Receiver Receiver;
  pid_t TxPid;
} ReceiverContext;

// Global receiver context
// There is a way to manage it without using global variable.
// That way utilizes linux's ucontext and it is way better.
// But I didn't manage to utilize it, so the straightforward
// solution (global variable for context) is used.
static ReceiverContext RxCtx;

static void RxSigHandler(int sigNum, siginfo_t* sigInfo, void*);

void RxDriver();