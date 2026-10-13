#pragma once
#include "../../../Core/Error/Status.h"
#include "Task.h"
#include "../Synchronization/Condition.h"
#include "../Thread/Thread.h"

typedef struct CONCURRENCY_TYPE(TThreadPool) {
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
} CONCURRENCY_TYPE(TThreadPool);

#define THREAD_POOL_INIT_PROTOTYPE                                            \
  static inline OPSTATUS THREAD_POOL_FUNC(Init)(                              \
      CONCURRENCY_TYPE(TThreadPool) *pool, CONCURRENCY_TYPE(TThread) *threads,\
      size_t threadCount, CONCURRENCY_TYPE(TTask) *queue, size_t queueCapacity)
#define THREAD_POOL_SUBMIT_PROTOTYPE                                          \
  static inline OPSTATUS THREAD_POOL_FUNC(Submit)(                            \
      CONCURRENCY_TYPE(TThreadPool) *pool, CONCURRENCY_TYPE(TTask) task)
#define THREAD_POOL_TRY_SUBMIT_PROTOTYPE                                      \
  static inline OPSTATUS THREAD_POOL_FUNC(TrySubmit)(                         \
      CONCURRENCY_TYPE(TThreadPool) *pool, CONCURRENCY_TYPE(TTask) task,      \
      bool *outSubmitted)
#define THREAD_POOL_SHUTDOWN_PROTOTYPE                                        \
  static inline OPSTATUS THREAD_POOL_FUNC(Shutdown)(                          \
      CONCURRENCY_TYPE(TThreadPool) *pool)
#define THREAD_POOL_DESTROY_PROTOTYPE                                         \
  static inline OPSTATUS THREAD_POOL_FUNC(Destroy)(                           \
      CONCURRENCY_TYPE(TThreadPool) *pool)

THREAD_POOL_INIT_PROTOTYPE;
THREAD_POOL_SUBMIT_PROTOTYPE;
THREAD_POOL_TRY_SUBMIT_PROTOTYPE;
THREAD_POOL_SHUTDOWN_PROTOTYPE;
THREAD_POOL_DESTROY_PROTOTYPE;

#include "Impl/ThreadPool.impl"
