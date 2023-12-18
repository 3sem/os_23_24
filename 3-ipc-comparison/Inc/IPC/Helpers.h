#pragma once

#include "IPCStatus.h"
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

IPCStatus ReadToBuf(char* buf, size_t bufSize, int srcFd, size_t* nRead,
                           int* readDone);

IPCStatus WriteToFd(const char* buf, size_t bufSize, int destFd,
                           size_t* nWrite);