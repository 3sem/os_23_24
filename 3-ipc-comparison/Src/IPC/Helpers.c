#include "IPC/Helpers.h"

IPCStatus ReadToBuf(char* buf, size_t bufSize, int srcFd, size_t* nRead,
                    int* readDone) {
  assert(buf);
  assert(nRead);

  size_t total = 0;

  while (total != bufSize) {
    int readSize = read(srcFd, buf + total, bufSize - total);

    if (readSize < 0) return IPC_ERRNO_ERROR;

    if (readSize == 0) {
      *readDone = 1;
      break;
    }

    total += readSize;
  }

  *nRead = total;
  return IPC_SUCCESS;
}

IPCStatus WriteToFd(const char* buf, size_t bufSize, int destFd,
                    size_t* nWrite) {
  assert(buf);
  assert(nWrite);

  size_t total = 0;

  while (total != bufSize) {
    int wrSize = write(destFd, buf + total, bufSize - total);

    if (wrSize < 0) return IPC_ERRNO_ERROR;
    if (wrSize == 0) return IPC_ZERO_WRITE;

    total += wrSize;
  }

  *nWrite = total;
  return IPC_SUCCESS;
}