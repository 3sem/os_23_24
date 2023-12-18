#pragma once

#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>

#include "Helpers.h"
#include "IPCStatus.h"
#include "ShMemSync.h"

typedef struct {
  char* Ptr;
  size_t Size;
} ShMemReceiver;

#ifdef __cplusplus
extern "C" {
#endif

IPCStatus RxInit(ShMemReceiver* self, size_t size);
IPCStatus RxOpen(ShMemReceiver* self, key_t key);
IPCStatus RxClose(ShMemReceiver* self);
IPCStatus RxReceive(ShMemReceiver* self, int destFd);

#ifdef __cplusplus
}
#endif