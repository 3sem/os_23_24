#include "IPC/ShMemRx.h"

IPCStatus RxInit(ShMemReceiver* self, size_t size) {
  if (!self) return IPC_BAD_ARG_PTR;
  self->Size = size + sizeof(ShMemHeader);

  return IPC_SUCCESS;
}

IPCStatus RxOpen(ShMemReceiver* self, key_t key) {
  if (!self) return IPC_BAD_ARG_PTR;

  int id = shmget(key, self->Size, 0);
  if (id < 0) return IPC_ERRNO_ERROR;

  char* newPtr = (char*)shmat(id, NULL, 0);
  if (newPtr == (void*)(-1)) return IPC_ERRNO_ERROR;

  self->Ptr = newPtr;
  return IPC_SUCCESS;
}

IPCStatus RxClose(ShMemReceiver* self) {
  if (!self) return IPC_BAD_ARG_PTR;

  if (shmdt(self->Ptr) < 0) return IPC_ERRNO_ERROR;

  return IPC_SUCCESS;
}

IPCStatus RxReceive(ShMemReceiver* self, int destFd) {
  if (!self) return IPC_BAD_ARG_PTR;

  size_t nWrite;
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

    while (header->State == SHM_SYNC_WRITE) pthread_cond_wait(full, mutex);

    status = WriteToFd(data, header->Size, destFd, &nWrite);
    assert(status == IPC_SUCCESS);

    if (header->State != SHM_SYNC_FINISH)
      header->State = SHM_SYNC_WRITE;
    else
      active = 0;

    pthread_cond_signal(empty);
    pthread_mutex_unlock(mutex);
  }
  return IPC_SUCCESS;
}
