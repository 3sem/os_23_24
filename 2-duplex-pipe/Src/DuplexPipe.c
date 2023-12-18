#include "DuplexPipe.h"

const char* GetErrorDescription(PipeStatus error) {
  switch (error) {
    case PIPE_SUCCESS:
      return "Success";
    case PIPE_BAD_ARG_PTR:
      return "Bad argument pointer";
    case PIPE_ERRNO_ERROR:
      return strerror(errno);
    case PIPE_STRATEGY_EMPTY:
      return "Pipe IO strategy is not defined";
    default:
      return "Unknown error";
  }
}

PipeStatus PipeInit(Pipe* self) {
  assert(self);

  int newDirectPipe[2];
  int newBackPipe[2];

  if (pipe(newDirectPipe) < 0) return PIPE_ERRNO_ERROR;

  if (pipe(newBackPipe) < 0) {
    close(newDirectPipe[0]);
    close(newDirectPipe[1]);
    return PIPE_ERRNO_ERROR;
  }

  memcpy(self->DirectFd, newDirectPipe, sizeof(self->DirectFd));
  memcpy(self->BackFd, newBackPipe, sizeof(self->BackFd));

  if (fcntl(self->DirectFd[0], F_SETFL,
            fcntl(self->DirectFd[0], F_GETFL) | O_NONBLOCK) < 0)
    return PIPE_ERRNO_ERROR;

  if (fcntl(self->BackFd[0], F_SETFL,
            fcntl(self->BackFd[0], F_GETFL) | O_NONBLOCK) < 0)
    return PIPE_ERRNO_ERROR;

  self->IO.Receive = NULL;
  self->IO.Send = NULL;

  return PIPE_SUCCESS;
}

static PipeStatus PipeReadFromFd(Pipe* self, int readFd, char* buf, size_t size,
                                 size_t* nReadPtr) {
  assert(self);
  assert(nReadPtr);

  size_t total = 0;

  while (total != size) {
    int ret = read(readFd, buf + total, size - total);

    if (ret < 0 && errno != EAGAIN) return PIPE_ERRNO_ERROR;
    if (ret < 0 && errno == EAGAIN) continue;
    if (ret == 0) break;
    total += ret;
  }

  *nReadPtr = total;
  return PIPE_SUCCESS;
}

static PipeStatus PipeWriteToFd(Pipe* self, int writeFd, const char* buf,
                                size_t size, size_t* nWritePtr) {
  assert(self);
  assert(nWritePtr);

  size_t total = 0;

  while (total != size) {
    int ret = write(writeFd, buf + total, size - total);

    if (ret < 0 && errno != EAGAIN) return PIPE_ERRNO_ERROR;
    if (ret < 0 && errno == EAGAIN) continue;
    if (ret == 0) break;
    total += ret;
  }

  *nWritePtr = total;
  return PIPE_SUCCESS;
}

static PipeStatus PipeSendParentImpl(Pipe* self, const char* buf, size_t size,
                                     size_t* nWritePtr) {
  assert(self);
  return PipeWriteToFd(self, self->DirectFd[1], buf, size, nWritePtr);
}

static PipeStatus PipeSendChildImpl(Pipe* self, const char* buf, size_t size,
                                    size_t* nWritePtr) {
  assert(self);
  return PipeWriteToFd(self, self->BackFd[1], buf, size, nWritePtr);
}

static PipeStatus PipeReceiveParentImpl(Pipe* self, char* buf, size_t size,
                                        size_t* nReadPtr) {
  assert(self);
  return PipeReadFromFd(self, self->BackFd[0], buf, size, nReadPtr);
}

static PipeStatus PipeReceiveChildImpl(Pipe* self, char* buf, size_t size,
                                       size_t* nReadPtr) {
  assert(self);
  return PipeReadFromFd(self, self->DirectFd[0], buf, size, nReadPtr);
}

static PipeStatus PipeFinishChildImpl(Pipe* self) {
  assert(self);
  close(self->BackFd[1]);
  return PIPE_SUCCESS;
}

static PipeStatus PipeFinishParentImpl(Pipe* self) {
  assert(self);
  close(self->DirectFd[1]);
  return PIPE_SUCCESS;
}

static PipeStatus PipeFinishParentImpl(Pipe* self);

PipeStatus PipeSend(Pipe* self, const char* buf, size_t size,
                    size_t* nWritePtr) {
  if (!self || !nWritePtr) return PIPE_BAD_ARG_PTR;
  if (self->IO.Send == NULL) return PIPE_STRATEGY_EMPTY;

  return self->IO.Send(self, buf, size, nWritePtr);
}

PipeStatus PipeReceive(Pipe* self, char* buf, size_t size, size_t* nReadPtr) {
  if (!self || !nReadPtr) return PIPE_BAD_ARG_PTR;
  if (self->IO.Send == NULL) return PIPE_STRATEGY_EMPTY;

  return self->IO.Receive(self, buf, size, nReadPtr);
}

PipeStatus PipeFinish(Pipe* self) {
  if (!self) return PIPE_BAD_ARG_PTR;
  if (self->IO.Finish == NULL) return PIPE_STRATEGY_EMPTY;

  return self->IO.Finish(self);
}

PipeStatus PipeSetParentStrategy(Pipe* self) {
  if (!self) return PIPE_BAD_ARG_PTR;
  close(self->BackFd[1]);
  close(self->DirectFd[0]);

  self->IO.Receive = PipeReceiveParentImpl;
  self->IO.Send = PipeSendParentImpl;
  self->IO.Finish = PipeFinishParentImpl;
  return PIPE_SUCCESS;
}

PipeStatus PipeSetChildStrategy(Pipe* self) {
  if (!self) return PIPE_BAD_ARG_PTR;

  close(self->BackFd[0]);
  close(self->DirectFd[1]);

  self->IO.Receive = PipeReceiveChildImpl;
  self->IO.Send = PipeSendChildImpl;
  self->IO.Finish = PipeFinishChildImpl;
  return PIPE_SUCCESS;
}

PipeStatus PipeDestroy(Pipe* self) {
  if (!self) return PIPE_BAD_ARG_PTR;

  close(self->DirectFd[0]);
  close(self->DirectFd[1]);
  close(self->BackFd[0]);
  close(self->BackFd[1]);

  return PIPE_SUCCESS;
}