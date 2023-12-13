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
} MQReceiver;

#ifdef __cplusplus
extern "C" {
#endif

IPCStatus RxInit(MQReceiver* self, size_t size);
IPCStatus RxOpen(MQReceiver* self, key_t key);
IPCStatus RxClose(MQReceiver* self);
IPCStatus RxReceive(MQReceiver* self, int destFd);

#ifdef __cplusplus
}
#endif