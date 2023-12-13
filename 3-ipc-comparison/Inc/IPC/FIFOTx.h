#pragma once

#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "IPCStatus.h"
#include "Helpers.h"

typedef struct {
  int Fd;
  char* Buf;
  size_t BufSize;
} FifoTransmitter;

#ifdef __cplusplus
extern "C" {
#endif

IPCStatus TxInit(FifoTransmitter* self, size_t bufSize);
IPCStatus TxOpen(FifoTransmitter* self, const char* path);
IPCStatus TxClose(FifoTransmitter* self);
IPCStatus TxTransmit(FifoTransmitter* self, int srcFd);

#ifdef __cplusplus
}
#endif