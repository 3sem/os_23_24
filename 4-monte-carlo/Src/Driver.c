#include "Driver.h"

DriverResult Driver(FunctionT function, size_t nWorkers, size_t nPoints) {
  if (sysconf(_SC_MONOTONIC_CLOCK) < 0)
    ExitWithError("Error: monotonic clock is not supported\n");

  MonteCarloArgs args;
  args.Function = function;

  args.Limits.Position.X = 0;
  args.Limits.Position.Y = 0;
  args.Limits.Size.X = 1;
  args.Limits.Size.Y = 1;

  args.NPoints = nPoints;

  struct timespec start, end;

  clock_gettime(CLOCK_MONOTONIC, &start);
  double result = MonteCarloMT(&args, nWorkers, nWorkers, 1);
  clock_gettime(CLOCK_MONOTONIC, &end);

  double time = end.tv_sec - start.tv_sec;
  time += (end.tv_nsec - start.tv_nsec) / 1000000000.0; // 10^9

  DriverResult ret = {result, time};
  return ret;
}