#include "Receiver.h"

TnStatus ReceiverInit(Receiver* self, int fd, size_t queueCapacity) {
  if (!self) return TNSTATUS(TN_BAD_ARG_PTR);

  self->Fd = fd;
  self->DoStop = 0;
  self->DoStart = 0;
  self->Status = TN_OK;
  self->Errno = 0;
  self->Remains = 0;

  TnStatus status = QueueInit(&self->Queue, queueCapacity);

  if (!TnStatusOk(status)) return status;

  int ret = pthread_create(&self->Thread, NULL, ReceiverMainLoop, self);
  if (ret != 0) {
    errno = ret;
    QueueDestroy(&self->Queue);
    return TNSTATUS(TN_ERRNO);
  }
  pthread_mutex_init(&self->Mutex, NULL);
  pthread_cond_init(&self->Cond, NULL);

  return TN_OK;
}

TnStatus ReceiverDestroy(Receiver* self) {
  if (!self) return TNSTATUS(TN_BAD_ARG_PTR);

  pthread_mutex_destroy(&self->Mutex);
  pthread_cond_destroy(&self->Cond);

  return QueueDestroy(&self->Queue);
}

static void ReceiverLock(Receiver* self) {
  assert(self);
  pthread_mutex_lock(&self->Mutex);
}

static void ReceiverUnlock(Receiver* self) {
  assert(self);
  pthread_mutex_unlock(&self->Mutex);
}

static void ReceiverSleep(Receiver* self) {
  assert(self);
  pthread_cond_wait(&self->Cond, &self->Mutex);
}

static void ReceiverSignal(Receiver* self) {
  assert(self);
  pthread_cond_signal(&self->Cond);
}

TnStatus ReceiverStart(Receiver* self, pid_t txPid) {
  if (!self) return TNSTATUS(TN_BAD_ARG_PTR);

  ReceiverLock(self);
  self->TxPid = txPid;
  self->DoStart = 1;
  ReceiverSignal(self);
  ReceiverUnlock(self);

  return TN_OK;
}

TnStatus ReceiverStop(Receiver* self) {
  if (!self) return TNSTATUS(TN_BAD_ARG_PTR);

  ReceiverLock(self);
  self->DoStop = 1;
  ReceiverSignal(self);
  ReceiverUnlock(self);

  return TN_OK;
}

static TnStatus WriteIntToFd(int fd, int val) {
  const char* buf = (const char*)(&val);
  size_t size = sizeof(int);
  size_t total = 0;

  while (total != size) {
    int ret = write(fd, buf + total, size - total);
    if (ret < 0) return TNSTATUS(TN_ERRNO);
    total += ret;
  }

  return TN_OK;
}

static TnStatus ReceiverFlushQueue(Receiver* self) {
  assert(self);

  int val;
  while (QueuePop(&self->Queue, &val).Code != TN_UNDERFLOW) {
    TnStatus status = WriteIntToFd(self->Fd, val);
    if (!TnStatusOk(status)) {
      self->Status = status;
      self->Errno = errno;
      return status;
    }
  }

  return TN_OK;
}

static TnStatus ReceiverFlushRemainder(Receiver* self) {
  assert(self);
  size_t total = 0;
  size_t size = self->Remains;
  const char* buf = self->Remainder;

  while (total < size) {
    int ret = write(self->Fd, buf + total, size - total);
    if (ret < 0) return TNSTATUS(TN_ERRNO);
    total += ret;
  }

  return TN_OK;
}

static void* ReceiverMainLoop(void* selfPtr) {
  assert(selfPtr);
  Receiver* self = (Receiver*)selfPtr;
  sigval_t sigVal;

  sigset_t sigset;
  sigfillset(&sigset);
  pthread_sigmask(SIG_BLOCK, &sigset, NULL) == 0;

  ReceiverLock(self);
  while (!self->DoStart) ReceiverSleep(self);
  ReceiverUnlock(self);

  sigVal.sival_int = CMD_CONNECT;
  TnStatus status = SendSignal(self->TxPid, CMD_SIGNUM, sigVal);
  TnStatusAssert(status);

  while (1) {
    ReceiverLock(self);

    while (self->Queue.Size == 0 && !self->DoStop) ReceiverSleep(self);

    ReceiverFlushQueue(self);
    ReceiverUnlock(self);

    if (self->DoStop) break;
  }

  ReceiverFlushRemainder(self);

  sigVal.sival_int = CMD_FINISH;
  status = SendSignal(self->TxPid, CMD_SIGNUM, sigVal);
  TnStatusAssert(status);

  self->Status = TN_OK;
  return NULL;
}

TnStatus ReceiverControlCallback(Receiver* self, int cmd, pid_t txPid) {
  assert(self);
  TnStatus status;

  switch (cmd) {
    case CMD_START:
      fprintf(stderr, "Receiver connecting to %d... \n", txPid);
      return ReceiverStart(self, txPid);
    case CMD_STOP:
      fprintf(stderr, "Receiver stopping...\n");
      return ReceiverStop(self);
    default:
      return TNSTATUS(TN_BAD_ARG_VAL);
  }
}

TnStatus ReceiverIntCallback(Receiver* self, int val) {
  assert(self);

  ReceiverLock(self);
  TnStatus status = QueuePush(&self->Queue, &val);
  TnStatusAssert(status);
  ReceiverSignal(self);
  ReceiverUnlock(self);

  return status;
}

TnStatus ReceiverCharCallback(Receiver* self, char val) {
  assert(self);

  ReceiverLock(self);
  assert(self->Remains < sizeof(self->Remainder) / sizeof(self->Remainder[0]));

  self->Remainder[self->Remains++] = val;
  ReceiverUnlock(self);

  return TN_OK;
}

TnStatus ReceiverSpin(Receiver* self) {
  if (!self) return TNSTATUS(TN_BAD_ARG_PTR);

  int ret = pthread_join(self->Thread, NULL);

  if (ret != 0) {
    errno = ret;
    return TNSTATUS(TN_ERRNO);
  }

  errno = self->Errno;
  return self->Status;
}