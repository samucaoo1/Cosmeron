#pragma once

#include "../../../Core/Error/Status.h"
#include "Task.h"
#include "../Synchronization/Condition.h"

typedef OPSTATUS (*TFutureFunction)(void *argument, void **result);

typedef struct TFuture {
  TMutex mutex;
  TCondition condition;
  void *result;
  OPSTATUS status;
  bool ready;
  bool initialized;
} TFuture;

inline static OPSTATUS FUTURE_FUNC(Init)(TFuture *future);
inline static OPSTATUS FUTURE_FUNC(Destroy)(TFuture *future);
inline static bool FUTURE_FUNC(IsReady)(const TFuture *future);
inline static OPSTATUS FUTURE_FUNC(Complete)(TFuture *future, void *result,
                                       OPSTATUS status);
inline static OPSTATUS FUTURE_FUNC(Wait)(TFuture *future);
inline static OPSTATUS FUTURE_FUNC(Get)(TFuture *future, void **result,
                                  OPSTATUS *resultStatus);

typedef struct TFutureTask {
  TFuture *future;
  TFutureFunction function;
  void *argument;
} TFutureTask;

inline static TTask FUTURE_FUNC(Task)(TFutureTask *futureTask, TFuture *future,
                                    TFutureFunction function, void *argument);

#include "Impl/Future.impl"
