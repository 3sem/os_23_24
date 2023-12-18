#pragma once

#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "Helpers.h"
#include "IPCStatus.h"

typedef struct {
  int Fd;
  char* Buf;
  size_t BufSize;
} FifoReceiver;

#ifdef __cplusplus
extern "C" {
#endif

IPCStatus RxInit(FifoReceiver* self, size_t bufSize);
IPCStatus RxOpen(FifoReceiver* self, const char* path);
IPCStatus RxClose(FifoReceiver* self);
IPCStatus RxReceive(FifoReceiver* self, int destFd);

#ifdef __cplusplus
}
#endif