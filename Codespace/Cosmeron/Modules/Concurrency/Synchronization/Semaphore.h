#pragma once
#include "../../../Core/Error/Status.h"

#include "Condition.h"

#include <limits.h>

typedef struct TSemaphore {
  TMutex mutex;
  TCondition condition;
  unsigned value;
} TSemaphore;

inline static OPSTATUS SEMAPHORE_FUNC(Init)(TSemaphore *semaphore, unsigned value);
inline static OPSTATUS SEMAPHORE_FUNC(Destroy)(TSemaphore *semaphore);
inline static OPSTATUS SEMAPHORE_FUNC(Wait)(TSemaphore *semaphore);
inline static OPSTATUS SEMAPHORE_FUNC(TryWait)(TSemaphore *semaphore, bool *acquired);
inline static OPSTATUS SEMAPHORE_FUNC(Post)(TSemaphore *semaphore);

#include "Impl/Semaphore.impl"
