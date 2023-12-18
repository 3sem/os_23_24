#pragma once

#include <pthread.h>

#include "IPCStatus.h"
#include <assert.h>

typedef struct {
  pthread_mutex_t Mutex;
  pthread_cond_t Full;
  pthread_cond_t Empty;

  struct {
    pthread_mutexattr_t Mutex;
    pthread_condattr_t Full;
    pthread_condattr_t Empty;
  } Attr;

} ShMemSync;

typedef enum {
  SHM_SYNC_READ = 0,
  SHM_SYNC_WRITE = 1,
  SHM_SYNC_FINISH = 2
} ShMemState;

typedef struct {
  size_t Size;
  ShMemState State;
  ShMemSync Sync;
} ShMemHeader;

#ifdef __cplusplus
extern "C" {
#endif

IPCStatus ShMemSyncInit(ShMemSync* self);
IPCStatus ShMemSyncDestroy(ShMemSync* self);

ShMemHeader* GetShMemHeader(char* buffer);
char* GetShMemData(char* buffer);
size_t GetShMemCapacity(size_t bufSize);

IPCStatus ShMemHeaderInit(ShMemHeader* header);
IPCStatus ShMemHeaderDestroy(ShMemHeader* header);

#ifdef __cplusplus
}
#endif
