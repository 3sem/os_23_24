#include "Queue.h"

TnStatus QueueInit(Queue* self, size_t capacity) {
  if (!self) return TNSTATUS(TN_BAD_ARG_PTR);
  if (!capacity) return TNSTATUS(TN_BAD_ARG_VAL);

  int* newBuffer = (int*)malloc(capacity * sizeof(int));
  if (!newBuffer) return TNSTATUS(TN_BAD_ALLOC);

  self->Buffer = newBuffer;
  self->Capacity = capacity;
  self->Size = 0;
  self->Head = 0;
  self->Tail = 0;

  return TN_OK;
}

TnStatus QueueDestroy(Queue* self) {
  if (!self) return TNSTATUS(TN_BAD_ARG_PTR);

  free(self->Buffer);

  return TN_OK;
}

static TnStatus QueueResize(Queue* self) {
  assert(self);
  assert(self->Size == self->Capacity);
  assert(self->Size);
  assert(self->Head == self->Tail);

  size_t newCapacity = self->Capacity * 2;

  int* newBuf = (int*)calloc(newCapacity, sizeof(int));
  if (!newBuf) return TNSTATUS(TN_BAD_ALLOC);

  size_t center = self->Head;
  size_t nRight = self->Capacity - center;
  size_t nLeft = center;

  memcpy(newBuf, self->Buffer + center, nRight * sizeof(int));
  memcpy(newBuf + nRight, self->Buffer, nLeft * sizeof(int));
  free(self->Buffer);

  self->Capacity = newCapacity;
  self->Buffer = newBuf;
  self->Tail = 0;
  self->Head = self->Size;

  return TN_OK;
}

TnStatus QueuePush(Queue* self, const int* val) {
  if (!self || !val) return TNSTATUS(TN_BAD_ARG_PTR);

  if (self->Size == self->Capacity) {
    TnStatus status = QueueResize(self);
    if (!TnStatusOk(status)) return status;
  }

  self->Buffer[self->Head] = *val;
  self->Head = (self->Head + 1) % self->Capacity;
  self->Size++;

  return TN_OK;
}

TnStatus QueuePop(Queue* self, int* val) {
  if (!self || !val) return TNSTATUS(TN_BAD_ARG_PTR);

  if (self->Size == 0) return TNSTATUS(TN_UNDERFLOW);

  *val = self->Buffer[self->Tail];
  self->Tail = (self->Tail + 1) % self->Capacity;
  self->Size--;

  return TN_OK;
}