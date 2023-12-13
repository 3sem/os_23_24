#include "TxDriver.h"

static void TxSigHandler(int sigNum, siginfo_t* sigInfo, void* ucontext) {
  assert(sigInfo);
  TnStatus status;

  int val = sigInfo->si_value.sival_int;
  pid_t pid = sigInfo->si_pid;

  if (pid != TxCtx.RxPid)
    ExitWithMessage("Another process tried to access transmitter");

  if (sigNum == CMD_SIGNUM) {
    status = TransmitterControlCallback(&TxCtx.Transmitter, val);
  } else
    ExitWithMessage("Unexpected signal received");

  TnStatusAssert(status);
}

void TxDriver(pid_t rxPid) {
  fprintf(stderr, "Starting transmitter on pid %d with RxPid=%d...\n", getpid(),
          rxPid);

  TxCtx.RxPid = rxPid;
  TnStatus status;

  status = TransmitterInit(&TxCtx.Transmitter, STDIN_FILENO, rxPid, 1);
  TnStatusAssert(status);

  struct sigaction sigAction;
  sigAction.sa_sigaction = TxSigHandler;
  sigAction.sa_flags = SA_SIGINFO;

  SigactionWrapper(CMD_SIGNUM, &sigAction);

  sigset_t set;
  sigfillset(&set);
  pthread_sigmask(SIG_UNBLOCK, &set, NULL);

  status = TransmitterStart(&TxCtx.Transmitter);
  TnStatusAssert(status);

  status = TransmitterSpin(&TxCtx.Transmitter);
  TnStatusAssert(status);
}