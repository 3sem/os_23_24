#pragma once

#include <sys/wait.h>

#include <iostream>
#include <string>

#include "BaseDriver.hpp"

#define MSG_KEY 69

double RunMsgDriver(size_t bufSize, const char* srcFile);