#include "Driver.h"

static const char* WrongArgsMsg =
    "Wrong arguments. Usage: ./RunBenchmark <NRuns> <NPoints>\n";

static const double Tolerance = 0.001;

double function(double x) { return x; }

int main(int argc, char* argv[]) {
  if (argc != 3) ExitWithError(WrongArgsMsg);

  size_t NRuns;
  size_t nPoints;

  if (sscanf(argv[1], "%zu", &NRuns) == EOF) ExitWithError(WrongArgsMsg);
  if (sscanf(argv[2], "%zu", &nPoints) == EOF) ExitWithError(WrongArgsMsg);

  size_t nWorkers = 1;

  printf("%zu\n", nPoints);

  for (int i = 0; i <= NRuns; ++i) {
    fprintf(stderr, "Run %d of %zu...\r", i, NRuns);
    DriverResult result = Driver(function, nWorkers, nPoints);
    if (abs(result.Result - 0.5) > Tolerance)
      ExitWithError("\nError: wrong result");

    printf("%zu %lf\n", nWorkers, result.TimeS);
    nWorkers *= 2;
  }

  fprintf(stderr, "\n");
}