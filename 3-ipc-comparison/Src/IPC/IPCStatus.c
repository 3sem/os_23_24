#include "IPC/IPCStatus.h"

const char* GetErrorDescription(IPCStatus status) {
  switch(status) {
    case IPC_SUCCESS:
      return "Success";
    case IPC_BAD_ARG_PTR:
      return "Bad argument pointer";
    case IPC_BAD_ALLOC:
      return "Failed to allocate memory";
    case IPC_ERRNO_ERROR:
      return strerror(errno);
    case IPC_ZERO_WRITE:
      return "write() returned zero";
    case IPC_NOT_INIT:
      return "IPC struct is not init";
    default:
      return "Unknown error";
  }
}