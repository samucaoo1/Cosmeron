#pragma once
#include "../../../Core/Error/Status.h"
#include "Task.h"
#include "../Synchronization/Condition.h"
#include "../Thread/Thread.h"

typedef struct CONCURRENCY_TYPE(CONCURRENCY_TYPE(TThread)Pool) {
  CONCURRENCY_TYPE(TThread) *threads;
  CONCURRENCY_TYPE(TTask) *queue;
  size_t threadCount;
  size_t queueCapacity;
  size_t queueHead;
  size_t queueSize;
  bool stopping;
  bool initialized;
  CONCURRENCY_TYPE(TMutex) mutex;
  CONCURRENCY_TYPE(TCondition) hasWork;
  CONCURRENCY_TYPE(TCondition) hasSpace;
} CONCURRENCY_TYPE(CONCURRENCY_TYPE(TThread)Pool);

static inline OPSTATUS THREAD_POOL_FUNC(Init)(CONCURRENCY_TYPE(CONCURRENCY_TYPE(TThread)Pool) *pool, CONCURRENCY_TYPE(TThread) *threads,
                                        size_t threadCount, CONCURRENCY_TYPE(TTask) *queue,
                                        size_t queueCapacity);
static inline OPSTATUS THREAD_POOL_FUNC(Submit)(CONCURRENCY_TYPE(CONCURRENCY_TYPE(TThread)Pool) *pool, CONCURRENCY_TYPE(TTask) task);
static inline OPSTATUS THREAD_POOL_FUNC(TrySubmit)(CONCURRENCY_TYPE(CONCURRENCY_TYPE(TThread)Pool) *pool, CONCURRENCY_TYPE(TTask) task, bool *outSubmitted);
static inline OPSTATUS THREAD_POOL_FUNC(Shutdown)(CONCURRENCY_TYPE(CONCURRENCY_TYPE(TThread)Pool) *pool);
static inline OPSTATUS THREAD_POOL_FUNC(Destroy)(CONCURRENCY_TYPE(CONCURRENCY_TYPE(TThread)Pool) *pool);

#include "Impl/ThreadPool.impl"
