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
} ShMemTransmitter;

#ifdef __cplusplus
extern "C" {
#endif

IPCStatus TxInit(ShMemTransmitter* self, size_t size);
IPCStatus TxOpen(ShMemTransmitter* self, key_t key);
IPCStatus TxClose(ShMemTransmitter* self);
IPCStatus TxTransmit(ShMemTransmitter* self, int srcFd);

#ifdef __cplusplus
}
#endif