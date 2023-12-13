#include "IPC/ShMemSync.h"

#define CHECK(cmd)          \
  error = cmd;              \
  if (error) {              \
    errno = error;          \
    return IPC_ERRNO_ERROR; \
  }

IPCStatus ShMemSyncInit(ShMemSync* self) {
  if (!self) return IPC_BAD_ARG_PTR;
  int error;

  CHECK(pthread_mutexattr_init(&self->Attr.Mutex));
  CHECK(
      pthread_mutexattr_setpshared(&self->Attr.Mutex, PTHREAD_PROCESS_SHARED));
  CHECK(pthread_mutex_init(&self->Mutex, &self->Attr.Mutex));

  CHECK(pthread_condattr_init(&self->Attr.Empty));
  CHECK(pthread_condattr_setpshared(&self->Attr.Empty, PTHREAD_PROCESS_SHARED));
  CHECK(pthread_cond_init(&self->Empty, &self->Attr.Empty));

  CHECK(pthread_condattr_init(&self->Attr.Full));
  CHECK(pthread_condattr_setpshared(&self->Attr.Full, PTHREAD_PROCESS_SHARED));
  CHECK(pthread_cond_init(&self->Full, &self->Attr.Full));

  return IPC_SUCCESS;
}

IPCStatus ShMemSyncDestroy(ShMemSync* self) {
  if (!self) return IPC_BAD_ARG_PTR;
  int error;

  CHECK(pthread_mutexattr_destroy(&self->Attr.Mutex));
  CHECK(pthread_condattr_destroy(&self->Attr.Full));
  CHECK(pthread_condattr_destroy(&self->Attr.Empty));

  CHECK(pthread_mutex_destroy(&self->Mutex));
  CHECK(pthread_cond_destroy(&self->Empty));
  CHECK(pthread_cond_destroy(&self->Full));

  return IPC_SUCCESS;
}

#undef CHECK

ShMemHeader* GetShMemHeader(char* buffer) {
  assert(buffer);
  return (ShMemHeader*)buffer;
}

char* GetShMemData(char* buffer) {
  assert(buffer);
  return (char*)(buffer + sizeof(ShMemHeader));
}

IPCStatus ShMemHeaderInit(ShMemHeader* header) {
  if (!header) return IPC_BAD_ARG_PTR;
  header->Size = 0;
  header->State = SHM_SYNC_WRITE;
  return ShMemSyncInit(&header->Sync);
}

IPCStatus ShMemHeaderDestroy(ShMemHeader* header) {
  if (!header) return IPC_BAD_ARG_PTR;
  return ShMemSyncDestroy(&header->Sync);
}

size_t GetShMemCapacity(size_t bufSize) {
  size_t capacity = bufSize - sizeof(ShMemHeader);
  assert(capacity < bufSize);  // Undeflow protection
  return capacity;
}