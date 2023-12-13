#pragma once
#include "MonteCarloMT.h"
#include <time.h>
#include <unistd.h>

typedef struct {
  double Result;
  double TimeS;
} DriverResult;

#ifdef __cplusplus
extern "C" {
#endif

DriverResult Driver(FunctionT function, size_t nWorkers, size_t nPoints);

#ifdef __cplusplus
}
#endif