#include "RxDriver.h"
#include "TxDriver.h"

int main() {
  sigset_t set;
  sigfillset(&set);
  sigprocmask(SIG_BLOCK, &set, NULL);

  pid_t pid = fork();

  if (pid < 0) ExitWithErrno("Fork");

  if (pid == 0) {  // Child
    RxDriver();
  } else {  // Parent
    TxDriver(pid);
  }
}