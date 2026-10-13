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
#define BARRIER_INIT_PROTOTYPE                                                \
  static inline OPSTATUS BARRIER_FUNC(Init)(                                  \
      CONCURRENCY_TYPE(TBarrier) *barrier, unsigned count)
#define BARRIER_DESTROY_PROTOTYPE                                             \
  static inline OPSTATUS BARRIER_FUNC(Destroy)(CONCURRENCY_TYPE(TBarrier) *barrier)
#define BARRIER_WAIT_PROTOTYPE                                                \
  static inline OPSTATUS BARRIER_FUNC(Wait)(CONCURRENCY_TYPE(TBarrier) *barrier)

BARRIER_INIT_PROTOTYPE;
BARRIER_DESTROY_PROTOTYPE;
BARRIER_WAIT_PROTOTYPE;
#include "Impl/Barrier.impl"
