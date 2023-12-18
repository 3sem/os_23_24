#pragma once

#include "MonteCarlo.h"
#include "ThreadPool/ThreadPool.h"
#include "math.h"

typedef struct {
  MonteCarloArgs Args;
  double Result;
} MCTask;

typedef struct {
  Vec2D Offset;
  Vec2D TileSize;
  
  size_t XCount;
  size_t YCount;

  size_t NTiles;
  size_t Current;
} Tiler;


void ExitWithError(const char* msg);
void AssertTnStatus(const char* msg, TnStatus status);
Tiler CreateTiler(Rectangle2D area, size_t xCount, size_t yCount);
int TilerGetNext(Tiler* tiler, Rectangle2D* tile);

static void MonteCarloAdapter(void* argsPtr, void* resultPtr);
double MonteCarloMT(MonteCarloArgs* args, size_t nWorkers, size_t tilesX,
                    size_t tilesY);