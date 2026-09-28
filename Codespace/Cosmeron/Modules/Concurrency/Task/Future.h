#pragma once

#include "../../../Core/Error/Status.h"
#include "Task.h"
#include "../Synchronization/Condition.h"

typedef OPSTATUS (*CONCURRENCY_TYPE(FutureFunction))(void *argument, void **outResult);

typedef struct CONCURRENCY_TYPE(TFuture) {
  CONCURRENCY_TYPE(TMutex) mutex;
  CONCURRENCY_TYPE(TCondition) condition;
  void *result;
  OPSTATUS status;
  bool ready;
  bool initialized;
} CONCURRENCY_TYPE(TFuture);

static inline OPSTATUS FUTURE_FUNC(Init)(CONCURRENCY_TYPE(TFuture) *future);
static inline OPSTATUS FUTURE_FUNC(Destroy)(CONCURRENCY_TYPE(TFuture) *future);
static inline bool FUTURE_FUNC(IsReady)(const CONCURRENCY_TYPE(TFuture) *future);
static inline OPSTATUS FUTURE_FUNC(Complete)(CONCURRENCY_TYPE(TFuture) *future, void *result,
                                       OPSTATUS status);
static inline OPSTATUS FUTURE_FUNC(Wait)(CONCURRENCY_TYPE(TFuture) *future);
static inline OPSTATUS FUTURE_FUNC(Get)(CONCURRENCY_TYPE(TFuture) *future, void **outResult,
                                  OPSTATUS *outStatus);

typedef struct CONCURRENCY_TYPE(TFutureTask) {
  CONCURRENCY_TYPE(TFuture) *future;
  CONCURRENCY_TYPE(FutureFunction) function;
  void *argument;
} CONCURRENCY_TYPE(TFutureTask);

static inline CONCURRENCY_TYPE(TTask) FUTURE_FUNC(Task)(CONCURRENCY_TYPE(TFutureTask) *futureTask, CONCURRENCY_TYPE(TFuture) *future,
                                    CONCURRENCY_TYPE(FutureFunction) function, void *argument);

#include "Impl/Future.impl"
