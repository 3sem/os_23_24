#include "Drivers/ShmDriver.hpp"

#include "IPC/ShMemRx.h"
#include "IPC/ShMemTx.h"

struct ShMemTxOpen {
  key_t Key;

  IPCStatus operator()(ShMemTransmitter* transmitter) {
    return TxOpen(transmitter, Key);
  }
};

struct ShMemRxOpen {
  key_t Key;

  IPCStatus operator()(ShMemReceiver* receiver) {
    return RxOpen(receiver, Key);
  }
};

static IPCStatus CreateShm(key_t key, size_t size) {
  int id;
  if ((id = shmget(key, size + sizeof(ShMemHeader), IPC_CREAT | 0666)) < 0) return IPC_ERRNO_ERROR;

  char* newPtr = (char*)shmat(id, NULL, 0);
  if (newPtr == (void*)(-1)) return IPC_ERRNO_ERROR;

  IPCStatus status = ShMemHeaderInit(GetShMemHeader(newPtr));
  if (status != IPC_SUCCESS) return status;

  if (shmdt(newPtr) < 0) return IPC_ERRNO_ERROR;
  return IPC_SUCCESS;
}

static IPCStatus DeleteShm(key_t key, size_t size) {
  int id = shmget(key, size + sizeof(ShMemHeader), 0);
  if (id < 0) return IPC_ERRNO_ERROR;

  char* ptr = (char*)shmat(id, NULL, 0);
  if (ptr == (void*)(-1)) return IPC_ERRNO_ERROR;

  IPCStatus status = ShMemHeaderDestroy(GetShMemHeader(ptr));
  if (status != IPC_SUCCESS) return status;

  if (shmdt(ptr) < 0) return IPC_ERRNO_ERROR;
  if (shmctl(id, IPC_RMID, NULL) < 0) return IPC_ERRNO_ERROR;

  return IPC_SUCCESS;
}

double RunShmDriver(size_t bufSize, const char* srcFile) {
  IPCStatus status;

  if ((status = CreateShm(SHM_KEY, bufSize)) != IPC_SUCCESS)
    PrintIPCErrorAndExit("Failed to create shared memory", status);

  ShMemTxOpen openTxFoo{SHM_KEY};
  ShMemRxOpen openRxFoo{SHM_KEY};

  clock_t startTime = clock();
  pid_t pid = fork();

  if (pid < 0)
    PrintErrnoAndExit("Failed to fork");
  else if (pid > 0) {  // Parent
    TxDriver<ShMemTransmitter>(bufSize, srcFile, openTxFoo);
    wait(NULL);

    if ((status = DeleteShm(SHM_KEY, bufSize)) != IPC_SUCCESS)
      PrintIPCErrorAndExit("Failed to delete shared memory", status);

    return double(clock() - startTime) / CLOCKS_PER_SEC;
  } else {  // Child
    RxDriver<ShMemReceiver>(bufSize, "out", openRxFoo);
    exit(0);
  }

  return 0;
}