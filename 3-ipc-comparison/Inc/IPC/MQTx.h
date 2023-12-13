#pragma once

#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "Helpers.h"
#include "IPCStatus.h"

typedef struct {
  int MqId;
  size_t Size;

  char* Msg;
} MQTransmitter;

#ifdef __cplusplus
extern "C" {
#endif

IPCStatus TxInit(MQTransmitter* self, size_t size);
IPCStatus TxOpen(MQTransmitter* self, key_t key);
IPCStatus TxClose(MQTransmitter* self);
IPCStatus TxTransmit(MQTransmitter* self, int srcFd);

#ifdef __cplusplus
}
#endif