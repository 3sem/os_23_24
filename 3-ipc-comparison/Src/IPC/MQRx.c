#include "IPC/MQRx.h"

IPCStatus RxInit(MQReceiver* self, size_t size) {
  if (!self) return IPC_BAD_ARG_PTR;

  char* newBuf = (char*)malloc(size * sizeof(char) + sizeof(long));
  if (!newBuf) return IPC_BAD_ALLOC;

  self->Msg = newBuf;
  self->Size = size;
  return IPC_SUCCESS;
}

IPCStatus RxOpen(MQReceiver* self, key_t key) {
  if (!self) return IPC_BAD_ARG_PTR;

  int id = msgget(key, 0);
  if (id < 0) return IPC_ERRNO_ERROR;

  self->MqId = id;

  return IPC_SUCCESS;
}

IPCStatus RxClose(MQReceiver* self) {
  if (!self) return IPC_BAD_ARG_PTR;
  free(self->Msg);
  return IPC_SUCCESS;
}

IPCStatus RxReceive(MQReceiver* self, int destFd) {
  if (!self) return IPC_BAD_ARG_PTR;

  int readDone = 0;
  size_t nRead;
  IPCStatus status;

  while (1) {
    int ret = msgrcv(self->MqId, self->Msg, self->Size, 0, 0);

    if (ret < 0) return IPC_ERRNO_ERROR;
    if (ret == 0) break;

    size_t nWrite;
    status = WriteToFd(self->Msg + sizeof(long), ret, destFd, &nWrite);
    if (status != IPC_SUCCESS) return status;
  }

  return IPC_SUCCESS;
}
