#include "Drivers/FifoDriver.hpp"

#include "IPC/FIFORx.h"
#include "IPC/FIFOTx.h"

struct FifoTxOpen {
  const char* FifoFile;

  IPCStatus operator()(FifoTransmitter* transmitter) {
    return TxOpen(transmitter, FifoFile);
  }
};

struct FifoRxOpen {
  const char* FifoFile;

  IPCStatus operator()(FifoReceiver* receiver) {
    return RxOpen(receiver, FifoFile);
  }
};

static IPCStatus CreateFifo(const char* path) {
  if (!path) return IPC_BAD_ARG_PTR;

  int ret = mknod(path, S_IFIFO | 0666, 0);
  if (ret < 0 && errno != EEXIST) return IPC_ERRNO_ERROR;

  return IPC_SUCCESS;
}

static IPCStatus DeleteFifo(const char* path) {
  if (!path) return IPC_BAD_ARG_PTR;

  return (unlink(path) < 0) ? IPC_ERRNO_ERROR : IPC_SUCCESS;
}

double RunFifoDriver(size_t bufSize, const char* srcFile) {
  IPCStatus status;

  if ((status = CreateFifo(FIFO_FILE)) != IPC_SUCCESS)
    PrintIPCErrorAndExit("Failed to create fifo", status);

  FifoTxOpen openTxFoo{FIFO_FILE};
  FifoRxOpen openRxFoo{FIFO_FILE};

  clock_t startTime = clock();
  pid_t pid = fork();

  if (pid < 0)
    PrintErrnoAndExit("Failed to fork");
  else if (pid > 0) {  // Parent
    TxDriver<FifoTransmitter>(bufSize, srcFile, openTxFoo);
    wait(NULL);

    if ((status = DeleteFifo(FIFO_FILE)) != IPC_SUCCESS)
      PrintIPCErrorAndExit("Failed to delete fifo", status);

    return double(clock() - startTime) / CLOCKS_PER_SEC;
  } else {  // Child
    RxDriver<FifoReceiver>(bufSize, "out", openRxFoo);
    exit(0);
  }

  return 0;
}