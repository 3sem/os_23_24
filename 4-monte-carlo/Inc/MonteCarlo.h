#pragma once
#include "stdio.h"
#include "assert.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"

typedef struct {
  double X;
  double Y;
} Vec2D;

typedef struct {
  Vec2D Position;
  Vec2D Size;
} Rectangle2D;

typedef double (*FunctionT)(double);

typedef struct {
  FunctionT Function;
  Rectangle2D Limits;
  size_t NPoints;
} MonteCarloArgs;

double MonteCarlo(MonteCarloArgs* args);