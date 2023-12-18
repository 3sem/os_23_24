#include "Drivers/MsgDriver.hpp"

#include "IPC/MQRx.h"
#include "IPC/MQTx.h"

struct MQTxOpen {
  key_t Key;

  IPCStatus operator()(MQTransmitter* transmitter) {
    return TxOpen(transmitter, Key);
  }
};

struct MQRxOpen {
  key_t Key;

  IPCStatus operator()(MQReceiver* receiver) {
    return RxOpen(receiver, Key);
  }
};

static IPCStatus CreateMsg(key_t key) {
  if (msgget(key, IPC_CREAT | 0666) < 0)
    return IPC_ERRNO_ERROR;

  return IPC_SUCCESS;
}

static IPCStatus DeleteMsg(key_t key) {
  int id = msgget(key, 0);
  if (id < 0) return IPC_ERRNO_ERROR;

  if (msgctl(id, IPC_RMID, NULL) < 0) return IPC_ERRNO_ERROR;

  return IPC_SUCCESS;
}

double RunMsgDriver(size_t bufSize, const char* srcFile) {
  IPCStatus status;

  if ((status = CreateMsg(MSG_KEY)) != IPC_SUCCESS)
    PrintIPCErrorAndExit("Failed to create msgqueue", status);

  MQTxOpen openTxFoo{MSG_KEY};
  MQRxOpen openRxFoo{MSG_KEY};

  clock_t startTime = clock();
  pid_t pid = fork();

  if (pid < 0)
    PrintErrnoAndExit("Failed to fork");
  else if (pid > 0) {  // Parent
    TxDriver<MQTransmitter>(bufSize, srcFile, openTxFoo);
    wait(NULL);

    if ((status = DeleteMsg(MSG_KEY)) != IPC_SUCCESS)
      PrintIPCErrorAndExit("Failed to delete msgqueue", status);

    return double(clock() - startTime) / CLOCKS_PER_SEC;
  } else {  // Child
    RxDriver<MQReceiver>(bufSize, "out", openRxFoo);
    exit(0);
  }

  return 0;
}