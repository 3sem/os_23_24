#include "Driver.h"

static const char* WrongArgsMsg =
    "Wrong arguments. Usage: ./4-monte-carlo <NWorkers> <NPoints>\n";

double function(double x) { return x; }

int main(int argc, char* argv[]) {
  if (argc != 3) ExitWithError(WrongArgsMsg);

  size_t nWorkers;
  size_t nPoints;

  if (sscanf(argv[1], "%zu", &nWorkers) == EOF) ExitWithError(WrongArgsMsg);

  if (sscanf(argv[2], "%zu", &nPoints) == EOF) ExitWithError(WrongArgsMsg);

  DriverResult result = Driver(function, nWorkers, nPoints);

  printf("Result: %lf\nTime: %lfs\n", result.Result, result.TimeS);
}