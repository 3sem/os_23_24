#include "Common.h"

TnStatus SendSignal(pid_t pid, int sigNo, sigval_t value) {
  while (1) {
    int ret = sigqueue(pid, sigNo, value);
    if (ret == 0) return TN_OK;
    if (ret == -1 && errno != EAGAIN) return TNSTATUS(TN_ERRNO);
  }
}

void ExitWithMessage(const char* msg) {
  fprintf(stderr, "Error: %s\n", msg);
  exit(1);
}

void ExitWithErrno(const char* msg) {
  fprintf(stderr, "Error: %s: %s\n", msg, strerror(errno));
  exit(1);
}

void SigactionWrapper(int sigNum, const struct sigaction* sigAction) {
  if (sigaction(sigNum, sigAction, NULL) != 0) ExitWithErrno("Sigaction");
}
