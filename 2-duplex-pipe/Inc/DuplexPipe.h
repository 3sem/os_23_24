#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>

struct PipeDecl;

typedef enum {
  PIPE_SUCCESS,
  PIPE_BAD_ARG_PTR,
  PIPE_ERRNO_ERROR,
  PIPE_STRATEGY_EMPTY
} PipeStatus;

typedef struct {
  PipeStatus (*Receive)(struct PipeDecl* self, char* buf, size_t size,
                        size_t* nReadPtr);
  PipeStatus (*Send)(struct PipeDecl* self, const char* buf, size_t size,
                     size_t* nWritePtr);
  PipeStatus (*Finish)(struct PipeDecl* self);
} IOStrategy;

typedef struct PipeDecl {
  int DirectFd[2];
  int BackFd[2];

  IOStrategy IO;
} Pipe;

const char* GetErrorDescription(PipeStatus error);

PipeStatus PipeInit(Pipe* self);
PipeStatus PipeSend(Pipe* self, const char* buf, size_t size,
                    size_t* nWritePtr);
PipeStatus PipeReceive(Pipe* self, char* buf, size_t size, size_t* nReadPtr);
PipeStatus PipeFinish(Pipe* self);
PipeStatus PipeSetParentStrategy(Pipe* self);
PipeStatus PipeSetChildStrategy(Pipe* self);
PipeStatus PipeDestroy(Pipe* self);

static PipeStatus PipeReadFromFd(Pipe* self, int readFd, char* buf, size_t size,
                                 size_t* nReadPtr);

static PipeStatus PipeWriteToFd(Pipe* self, int writeFd, const char* buf,
                                size_t size, size_t* nWritePtr);

static PipeStatus PipeSendParentImpl(Pipe* self, const char* buf, size_t size,
                                     size_t* nWritePtr);
static PipeStatus PipeSendChildImpl(Pipe* self, const char* buf, size_t size,
                                    size_t* nWritePtr);
static PipeStatus PipeReceiveParentImpl(Pipe* self, char* buf, size_t size,
                                        size_t* nReadPtr);
static PipeStatus PipeReceiveChildImpl(Pipe* self, char* buf, size_t size,
                                       size_t* nReadPtr);

static PipeStatus PipeFinishChildImpl(Pipe* self);
static PipeStatus PipeFinishParentImpl(Pipe* self);