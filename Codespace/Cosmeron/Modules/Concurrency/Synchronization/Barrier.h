#pragma once
#include "../../../Core/Error/Status.h"
#include "Condition.h"
typedef struct TBarrier {
  TMutex mutex;
  TCondition condition;
  unsigned threshold;
  unsigned waiting;
  unsigned generation;
  bool broken;
} TBarrier;
inline static OPSTATUS BARRIER_FUNC(Init)(TBarrier *barrier, unsigned count);
inline static OPSTATUS BARRIER_FUNC(Destroy)(TBarrier *barrier);
inline static OPSTATUS BARRIER_FUNC(Wait)(TBarrier *barrier);
#include "Impl/Barrier.impl"
