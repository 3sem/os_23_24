#include "IPC/ShMemTx.h"

IPCStatus TxInit(ShMemTransmitter* self, size_t size) {
  if (!self) return IPC_BAD_ARG_PTR;
  self->Size = size + sizeof(ShMemHeader);

  return IPC_SUCCESS;
}

IPCStatus TxOpen(ShMemTransmitter* self, key_t key) {
  if (!self) return IPC_BAD_ARG_PTR;

  int id = shmget(key, self->Size, 0);
  if (id < 0) return IPC_ERRNO_ERROR;

  char* newPtr = (char*)shmat(id, NULL, 0);
  if (newPtr == (void*)(-1)) return IPC_ERRNO_ERROR;

  self->Ptr = newPtr;
  return IPC_SUCCESS;
}

IPCStatus TxClose(ShMemTransmitter* self) {
  if (!self) return IPC_BAD_ARG_PTR;

  if (shmdt(self->Ptr) < 0) return IPC_ERRNO_ERROR;

  return IPC_SUCCESS;
}

IPCStatus TxTransmit(ShMemTransmitter* self, int srcFd) {
  if (!self) return IPC_BAD_ARG_PTR;

  size_t nRead;
  int readDone = 0;
  IPCStatus status;

  ShMemHeader* header = GetShMemHeader(self->Ptr);
  char* data = GetShMemData(self->Ptr);
  size_t capacity = GetShMemCapacity(self->Size);

  pthread_mutex_t* mutex = &header->Sync.Mutex;
  pthread_cond_t* empty = &header->Sync.Empty;
  pthread_cond_t* full = &header->Sync.Full;

  int active = 1;

  while (active) {
    pthread_mutex_lock(mutex);

    while (header->State == SHM_SYNC_READ) pthread_cond_wait(empty, mutex);

    status = ReadToBuf(data, capacity, srcFd, &nRead, &readDone);
    assert(status == IPC_SUCCESS);

    header->Size = nRead;

    if (readDone) {
      header->State = SHM_SYNC_FINISH;
      active = 0;
    } else
      header->State = SHM_SYNC_READ;

    pthread_cond_signal(full);
    pthread_mutex_unlock(mutex);
  }

  return IPC_SUCCESS;
}
