#pragma once

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "BaseDriver.hpp"

#define FIFO_FILE "FIFO"

double RunFifoDriver(size_t bufSize, const char* srcFile);