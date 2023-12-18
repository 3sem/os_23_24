#pragma once

#include <errno.h>
#include <string.h>

typedef enum {
  IPC_SUCCESS,
  IPC_BAD_ARG_PTR,
  IPC_BAD_ALLOC,
  IPC_ERRNO_ERROR,
  IPC_ZERO_WRITE,
  IPC_NOT_INIT
} IPCStatus;

#ifdef __cplusplus
extern "C" {
#endif

const char* GetErrorDescription(IPCStatus status);

#ifdef __cplusplus
}
#endif