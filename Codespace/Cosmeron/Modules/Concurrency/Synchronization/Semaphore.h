#pragma once
#include "../../../Core/Error/Status.h"

#include "Condition.h"

#include <limits.h>

typedef struct CONCURRENCY_TYPE(TSemaphore) {
  CONCURRENCY_TYPE(TMutex) mutex;
  CONCURRENCY_TYPE(TCondition) condition;
  unsigned value;
} CONCURRENCY_TYPE(TSemaphore);

#define SEMAPHORE_INIT_PROTOTYPE                                              \
  static inline OPSTATUS SEMAPHORE_FUNC(Init)(                                \
      CONCURRENCY_TYPE(TSemaphore) *semaphore, unsigned value)
#define SEMAPHORE_DESTROY_PROTOTYPE                                           \
  static inline OPSTATUS SEMAPHORE_FUNC(Destroy)(                             \
      CONCURRENCY_TYPE(TSemaphore) *semaphore)
#define SEMAPHORE_WAIT_PROTOTYPE                                              \
  static inline OPSTATUS SEMAPHORE_FUNC(Wait)(                                \
      CONCURRENCY_TYPE(TSemaphore) *semaphore)
#define SEMAPHORE_TRY_WAIT_PROTOTYPE                                          \
  static inline OPSTATUS SEMAPHORE_FUNC(TryWait)(                             \
      CONCURRENCY_TYPE(TSemaphore) *semaphore, bool *outAcquired)
#define SEMAPHORE_POST_PROTOTYPE                                              \
  static inline OPSTATUS SEMAPHORE_FUNC(Post)(                                \
      CONCURRENCY_TYPE(TSemaphore) *semaphore)

SEMAPHORE_INIT_PROTOTYPE;
SEMAPHORE_DESTROY_PROTOTYPE;
SEMAPHORE_WAIT_PROTOTYPE;
SEMAPHORE_TRY_WAIT_PROTOTYPE;
SEMAPHORE_POST_PROTOTYPE;

#include "Impl/Semaphore.impl"
