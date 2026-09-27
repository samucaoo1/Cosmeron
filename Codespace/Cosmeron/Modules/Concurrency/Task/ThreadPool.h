#pragma once
#include "../../../Core/Error/Status.h"
#include "Task.h"
#include "../Synchronization/Condition.h"
#include "../Thread/Thread.h"

typedef struct TThreadPool {
  TThread *threads;
  TTask *queue;
  size_t threadCount;
  size_t queueCapacity;
  size_t queueHead;
  size_t queueSize;
  bool stopping;
  bool initialized;
  TMutex mutex;
  TCondition hasWork;
  TCondition hasSpace;
} TThreadPool;

inline static OPSTATUS THREAD_POOL_FUNC(Init)(TThreadPool *pool, TThread *threads,
                                        size_t threadCount, TTask *queue,
                                        size_t queueCapacity);
inline static OPSTATUS THREAD_POOL_FUNC(Submit)(TThreadPool *pool, TTask task);
inline static OPSTATUS THREAD_POOL_FUNC(TrySubmit)(TThreadPool *pool, TTask task, bool *submitted);
inline static OPSTATUS THREAD_POOL_FUNC(Shutdown)(TThreadPool *pool);
inline static OPSTATUS THREAD_POOL_FUNC(Destroy)(TThreadPool *pool);

#include "Impl/ThreadPool.impl"
