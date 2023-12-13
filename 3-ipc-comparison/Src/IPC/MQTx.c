#include "IPC/MQTx.h"

static IPCStatus TxSetMessageType(MQTransmitter* self, long type) {
  if (!self) return IPC_BAD_ARG_PTR;
  if (!self->Msg) return IPC_NOT_INIT;

  *((long*)self->Msg) = type;
  return IPC_SUCCESS;
}

IPCStatus TxInit(MQTransmitter* self, size_t size) {
  if (!self) return IPC_BAD_ARG_PTR;

  char* newBuf = (char*)malloc(size * sizeof(char) + sizeof(long));
  if (!newBuf) return IPC_BAD_ALLOC;

  self->Msg = newBuf;
  TxSetMessageType(self, 1);
  self->Size = size;
  return IPC_SUCCESS;
}

IPCStatus TxOpen(MQTransmitter* self, key_t key) {
  if (!self) return IPC_BAD_ARG_PTR;

  int id = msgget(key, 0);
  if (id < 0) return IPC_ERRNO_ERROR;

  self->MqId = id;

  return IPC_SUCCESS;
}

IPCStatus TxClose(MQTransmitter* self) {
  if (!self) return IPC_BAD_ARG_PTR;
  free(self->Msg);
  return IPC_SUCCESS;
}

IPCStatus TxTransmit(MQTransmitter* self, int srcFd) {
  if (!self) return IPC_BAD_ARG_PTR;

  int readDone = 0;
  size_t nRead;
  IPCStatus status;

  while (!readDone) {
    status = ReadToBuf(self->Msg + sizeof(long), self->Size, srcFd, &nRead,
                       &readDone);

    if (msgsnd(self->MqId, self->Msg, nRead, 0) < 0) return IPC_ERRNO_ERROR;
  }

  if (msgsnd(self->MqId, self->Msg, 0, 0) < 0) return IPC_ERRNO_ERROR;

  return IPC_SUCCESS;
}
