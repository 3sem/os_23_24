#pragma once
#include <stdlib.h>

#include "TnStatus.h"
#include "assert.h"

typedef struct {
  int* Buffer;
  size_t Size;
  size_t Capacity;
  size_t Head;
  size_t Tail;
} Queue;

TnStatus QueueInit(Queue* self, size_t capacity);
TnStatus QueueDestroy(Queue* self);
TnStatus QueuePush(Queue* self, const int* val);
TnStatus QueuePop(Queue* self, int* val);

static TnStatus QueueResize(Queue* self);