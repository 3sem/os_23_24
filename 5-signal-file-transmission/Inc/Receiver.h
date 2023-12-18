#pragma once

#define _GNU_SOURCE

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#include "Common.h"
#include "Queue.h"

typedef struct {
  Queue Queue;

  pthread_t Thread;
  pthread_mutex_t Mutex;
  pthread_cond_t Cond;

  TnStatus Status;
  int Errno;

  int DoStop;
  int DoStart;
  int Fd;
  pid_t TxPid;

  char Remainder[sizeof(int)];
  size_t Remains;
} Receiver;

TnStatus ReceiverInit(Receiver* self, int fd, size_t queueCapacity);
TnStatus ReceiverDestroy(Receiver* self);

TnStatus ReceiverStart(Receiver* self, pid_t txPid);
TnStatus ReceiverStop(Receiver* self);

TnStatus ReceiverControlCallback(Receiver* self, int cmd, pid_t txPid);
TnStatus ReceiverIntCallback(Receiver* self, int val);
TnStatus ReceiverCharCallback(Receiver* self, char val);
TnStatus ReceiverSpin(Receiver* self);

static TnStatus WriteIntToFd(int fd, int val);
static TnStatus ReceiverFlushQueue(Receiver* self);
static TnStatus ReceiverFlushRemainder(Receiver* self);

static void ReceiverLock(Receiver* self);
static void ReceiverUnlock(Receiver* self);
static void ReceiverSleep(Receiver* self);
static void ReceiverSignal(Receiver* self);

static void* ReceiverMainLoop(void* selfPtr);