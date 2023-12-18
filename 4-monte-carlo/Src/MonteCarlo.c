#include "MonteCarlo.h"

double MonteCarlo(MonteCarloArgs* args) {
  assert(args);
  assert(args->Function);
  unsigned int seed = time(NULL);

  size_t count = 0;
  for (int i = 0; i < args->NPoints; ++i) {
    double dx = args->Limits.Size.X * (double)(rand_r(&seed)) / RAND_MAX;
    double dy = args->Limits.Size.Y * (double)(rand_r(&seed)) / RAND_MAX;

    double x = args->Limits.Position.X + dx;
    double y = args->Limits.Position.Y + dy;

    if (y < args->Function(x))
      count++;
  }

  double areaFilled = (double)(count) / (double)(args->NPoints);
  double area = args->Limits.Size.X * args->Limits.Size.Y * areaFilled;
  return area;
}