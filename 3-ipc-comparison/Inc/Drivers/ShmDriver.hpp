#pragma once

#include <sys/wait.h>

#include <iostream>
#include <string>

#include "BaseDriver.hpp"

#define SHM_KEY 42

double RunShmDriver(size_t bufSize, const char* srcFile);