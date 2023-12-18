#include "Transmitter.h"

TnStatus TransmitterInit(Transmitter* self, int fd, pid_t rxPid,
                         size_t pollPeriod) {
  if (!self) return TNSTATUS(TN_BAD_ARG_PTR);

  self->Fd = fd;
  self->RxPid = rxPid;
  self->Connected = 0;
  self->RxFinished = 0;
  self->PollPeriod = pollPeriod;
  self->Status = TN_OK;
  self->Errno = 0;

  pthread_mutex_init(&self->Mutex, NULL);
  pthread_cond_init(&self->Cond, NULL);

  return TN_OK;
}

TnStatus TransmitterDestroy(Transmitter* self) {
  if (!self) return TNSTATUS(TN_BAD_ARG_PTR);

  pthread_mutex_destroy(&self->Mutex);
  pthread_cond_destroy(&self->Cond);

  return TN_OK;
}

static void TransmitterLock(Transmitter* self) {
  assert(self);
  pthread_mutex_lock(&self->Mutex);
}

static void TransmitterUnlock(Transmitter* self) {
  assert(self);
  pthread_mutex_unlock(&self->Mutex);
}

static void TransmitterSignal(Transmitter* self) {
  assert(self);
  pthread_cond_signal(&self->Cond);
}

static void TransmitterSleep(Transmitter* self) {
  assert(self);
  pthread_cond_wait(&self->Cond, &self->Mutex);
}

static void TransmitterSleepFor(Transmitter* self, size_t seconds) {
  assert(self);

  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_sec += seconds;

  pthread_cond_timedwait(&self->Cond, &self->Mutex, &ts);
}

static void TransmitterSendCmd(Transmitter* self, int cmd) {
  assert(self);

  sigval_t sigVal;
  sigVal.sival_int = cmd;
  TnStatus status = SendSignal(self->RxPid, CMD_SIGNUM, sigVal);
  TnStatusAssert(status);
}

static void TransmitterSendInt(Transmitter* self, int val) {
  assert(self);

  sigval_t sigVal;
  sigVal.sival_int = val;
  TnStatus status = SendSignal(self->RxPid, DATA_INT_SIGNUM, sigVal);
  TnStatusAssert(status);
}

static void TransmitterSendChar(Transmitter* self, char val) {
  assert(self);

  sigval_t sigVal;
  sigVal.sival_int = val;
  TnStatus status = SendSignal(self->RxPid, DATA_CHAR_SIGNUM, sigVal);
  TnStatusAssert(status);
}

TnStatus TransmitterControlCallback(Transmitter* self, int val) {
  if (!self) return TNSTATUS(TN_BAD_ARG_PTR);

  if (val == CMD_CONNECT) {
    if (self->Connected) return TNSTATUS(TN_FSM_WRONG_STATE);

    TransmitterLock(self);
    self->Connected = 1;
    TransmitterSignal(self);
    TransmitterUnlock(self);
    return TN_OK;
  }
  if (val == CMD_FINISH) {
    if (self->RxFinished) return TNSTATUS(TN_FSM_WRONG_STATE);
    TransmitterLock(self);
    self->RxFinished = 1;
    TransmitterSignal(self);
    TransmitterUnlock(self);
    return TN_OK;
  }

  return TNSTATUS(TN_BAD_ARG_VAL);
}

static void TransmitterWaitReceiverConnection(Transmitter* self) {
  size_t nAttempt = 1;
  fprintf(stderr, "Waiting for connection...\n");

  TransmitterLock(self);
  while (!self->Connected) {
    if (nAttempt > 1) fprintf(stderr, "Attempt #%zu...\r", nAttempt++);

    TransmitterSendCmd(self, CMD_START);
    TransmitterSleepFor(self, self->PollPeriod);
  }
  TransmitterUnlock(self);

  fprintf(stderr, "\n");
}

static void TransmitterWaitReceiverFinish(Transmitter* self) {
  TransmitterSendCmd(self, CMD_STOP);
  fprintf(stderr, "Waiting for receiver to finish...\n");
  TransmitterLock(self);
  while (!self->RxFinished) TransmitterSleep(self);
  TransmitterUnlock(self);
}

static TnStatus ReadFromFdToInt(int fd, int* val, int* hasEof, int* remains) {
  if (!val || !hasEof || !remains) return TNSTATUS(TN_BAD_ARG_PTR);
  char* buf = (char*)val;
  size_t size = sizeof(int);
  size_t total = 0;

  while (total < size) {
    int ret = read(fd, buf + total, size - total);
    if (!ret) {
      *remains = total;
      *hasEof = 1;
      break;
    }
    if (ret < 0) return TNSTATUS(TN_ERRNO);
    total += ret;
  }

  return TN_OK;
}

static void* TransmitterMainLoop(void* selfPtr) {
  assert(selfPtr);
  Transmitter* self = (Transmitter*)selfPtr;
  TnStatus status;

  TransmitterWaitReceiverConnection(self);

  int val, remains, hasEof = 0;

  while (1) {
    val = 0;
    status = ReadFromFdToInt(self->Fd, &val, &hasEof, &remains);
    if (!TnStatusOk(status)) break;
    if (hasEof) break;

    TransmitterSendInt(self, val);
  }

  char* remainder = (char*)&val;

  for (int i = 0; i < remains; ++i) {
    // cppcheck-suppress objectIndex
    TransmitterSendChar(self, remainder[i]);
  }

  TransmitterWaitReceiverFinish(self);

  self->Status = status;
  self->Errno = errno;
}

TnStatus TransmitterStart(Transmitter* self) {
  if (!self) return TNSTATUS(TN_BAD_ARG_PTR);

  int ret = pthread_create(&self->Thread, NULL, TransmitterMainLoop, self);

  if (ret != 0) {
    errno = ret;
    return TNSTATUS(TN_ERRNO);
  }

  return TN_OK;
}

TnStatus TransmitterSpin(Transmitter* self) {
  if (!self) return TNSTATUS(TN_BAD_ARG_PTR);

  int ret = pthread_join(self->Thread, NULL);

  if (ret != 0) {
    errno = ret;
    return TNSTATUS(TN_ERRNO);
  }

  errno = self->Errno;
  return self->Status;
}