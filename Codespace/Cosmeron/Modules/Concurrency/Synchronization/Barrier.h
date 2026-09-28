#pragma once
#include "../../../Core/Error/Status.h"
#include "Condition.h"
typedef struct CONCURRENCY_TYPE(TBarrier) {
  CONCURRENCY_TYPE(TMutex) mutex;
  CONCURRENCY_TYPE(TCondition) condition;
  unsigned threshold;
  unsigned waiting;
  unsigned generation;
  bool broken;
} CONCURRENCY_TYPE(TBarrier);
static inline OPSTATUS BARRIER_FUNC(Init)(CONCURRENCY_TYPE(TBarrier) *barrier, unsigned count);
static inline OPSTATUS BARRIER_FUNC(Destroy)(CONCURRENCY_TYPE(TBarrier) *barrier);
static inline OPSTATUS BARRIER_FUNC(Wait)(CONCURRENCY_TYPE(TBarrier) *barrier);
#include "Impl/Barrier.impl"
