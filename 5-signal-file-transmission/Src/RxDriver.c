#include "RxDriver.h"

static void RxSigHandler(int sigNum, siginfo_t* sigInfo, void* ucontext) {
  assert(sigInfo);
  TnStatus status;

  int val = sigInfo->si_value.sival_int;
  pid_t pid = sigInfo->si_pid;

  if (RxCtx.TxPid && RxCtx.TxPid != pid)
    ExitWithMessage("Other transmitter tried to send message");

  if (sigNum == CMD_SIGNUM) {
    if (RxCtx.TxPid == 0) RxCtx.TxPid = pid;
    status = ReceiverControlCallback(&RxCtx.Receiver, val, pid);
  } else if (sigNum == DATA_INT_SIGNUM) {
    if (RxCtx.TxPid == 0)
      ExitWithMessage("Receiver not started but data received\n");
    status = ReceiverIntCallback(&RxCtx.Receiver, val);
  } else if (sigNum == DATA_CHAR_SIGNUM) {
    if (RxCtx.TxPid == 0)
      ExitWithMessage("Receiver not started but data received\n");
    status = ReceiverCharCallback(&RxCtx.Receiver, (char)val);
  } else
    ExitWithMessage("Unexpected signal received\n");

  TnStatusAssert(status);
}

void RxDriver() {
  fprintf(stderr, "Starting receiver on pid %d...\n", getpid());

  RxCtx.TxPid = 0;
  TnStatus status;

  int sigQueueCapacity = sysconf(_SC_SIGQUEUE_MAX);
  status = ReceiverInit(&RxCtx.Receiver, STDOUT_FILENO, sigQueueCapacity);

  TnStatusAssert(status);

  struct sigaction sigAction;
  sigAction.sa_sigaction = RxSigHandler;
  sigAction.sa_flags = SA_SIGINFO;

  sigemptyset(&sigAction.sa_mask);
  sigaddset(&sigAction.sa_mask, DATA_INT_SIGNUM);
  sigaddset(&sigAction.sa_mask, DATA_CHAR_SIGNUM);
  sigaddset(&sigAction.sa_mask, CMD_SIGNUM);

  SigactionWrapper(DATA_INT_SIGNUM, &sigAction);
  SigactionWrapper(DATA_CHAR_SIGNUM, &sigAction);
  SigactionWrapper(CMD_SIGNUM, &sigAction);

  sigset_t set;
  sigfillset(&set);
  pthread_sigmask(SIG_UNBLOCK, &set, NULL);

  TnStatusAssert(status);

  status = ReceiverSpin(&RxCtx.Receiver);
  TnStatusAssert(status);
}