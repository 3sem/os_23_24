#pragma once

#define _GNU_SOURCE

#include <assert.h>
#include <pthread.h>
#include <unistd.h>

#include "Common.h"
#include "TnStatus.h"

typedef struct {
  pthread_mutex_t Mutex;
  pthread_cond_t Cond;
  pthread_t Thread;

  pid_t RxPid;
  int Connected;
  int RxFinished;
  int Fd;

  TnStatus Status;
  int Errno;

  size_t PollPeriod;
} Transmitter;

TnStatus TransmitterInit(Transmitter* self, int fd, pid_t rxPid,
                         size_t pollPeriod);

TnStatus TransmitterDestroy(Transmitter* self);
TnStatus TransmitterControlCallback(Transmitter* self, int val);

TnStatus TransmitterStart(Transmitter* self);
TnStatus TransmitterSpin(Transmitter* self);

static void TransmitterLock(Transmitter* self);
static void TransmitterUnlock(Transmitter* self);
static void TransmitterSignal(Transmitter* self);
static void TransmitterSleep(Transmitter* self);
static void TransmitterSleepFor(Transmitter* self, size_t seconds);

static void TransmitterWaitReceiverConnection(Transmitter* self);
static void TransmitterWaitReceiverFinish(Transmitter* self);

static void TransmitterSendCmd(Transmitter* self, int cmd);
static void TransmitterSendInt(Transmitter* self, int val);
static void TransmitterSendChar(Transmitter* self, char val);

static void* TransmitterMainLoop(void* selfPtr);
static TnStatus ReadFromFdToInt(int fd, int* val, int* hasEof, int* remains);