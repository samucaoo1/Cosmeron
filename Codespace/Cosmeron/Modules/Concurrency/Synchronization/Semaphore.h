#pragma once
#include "../../../Core/Error/Status.h"

#include "Condition.h"

#include <limits.h>

typedef struct CONCURRENCY_TYPE(TSemaphore) {
  CONCURRENCY_TYPE(TMutex) mutex;
  CONCURRENCY_TYPE(TCondition) condition;
  unsigned value;
} CONCURRENCY_TYPE(TSemaphore);

static inline OPSTATUS SEMAPHORE_FUNC(Init)(CONCURRENCY_TYPE(TSemaphore) *semaphore, unsigned value);
static inline OPSTATUS SEMAPHORE_FUNC(Destroy)(CONCURRENCY_TYPE(TSemaphore) *semaphore);
static inline OPSTATUS SEMAPHORE_FUNC(Wait)(CONCURRENCY_TYPE(TSemaphore) *semaphore);
static inline OPSTATUS SEMAPHORE_FUNC(TryWait)(CONCURRENCY_TYPE(TSemaphore) *semaphore, bool *outAcquired);
static inline OPSTATUS SEMAPHORE_FUNC(Post)(CONCURRENCY_TYPE(TSemaphore) *semaphore);

#include "Impl/Semaphore.impl"
