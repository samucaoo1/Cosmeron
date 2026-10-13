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

#define FUTURE_INIT_PROTOTYPE                                                 \
  static inline OPSTATUS FUTURE_FUNC(Init)(CONCURRENCY_TYPE(TFuture) *future)
#define FUTURE_DESTROY_PROTOTYPE                                              \
  static inline OPSTATUS FUTURE_FUNC(Destroy)(CONCURRENCY_TYPE(TFuture) *future)
#define FUTURE_IS_READY_PROTOTYPE                                             \
  static inline bool FUTURE_FUNC(IsReady)(const CONCURRENCY_TYPE(TFuture) *future)
#define FUTURE_COMPLETE_PROTOTYPE                                             \
  static inline OPSTATUS FUTURE_FUNC(Complete)(                               \
      CONCURRENCY_TYPE(TFuture) *future, void *result, OPSTATUS resultStatus)
#define FUTURE_WAIT_PROTOTYPE                                                 \
  static inline OPSTATUS FUTURE_FUNC(Wait)(CONCURRENCY_TYPE(TFuture) *future)
#define FUTURE_GET_PROTOTYPE                                                  \
  static inline OPSTATUS FUTURE_FUNC(Get)(                                    \
      CONCURRENCY_TYPE(TFuture) *future, void **outResult, OPSTATUS *outStatus)

FUTURE_INIT_PROTOTYPE;
FUTURE_DESTROY_PROTOTYPE;
FUTURE_IS_READY_PROTOTYPE;
FUTURE_COMPLETE_PROTOTYPE;
FUTURE_WAIT_PROTOTYPE;
FUTURE_GET_PROTOTYPE;

typedef struct CONCURRENCY_TYPE(TFutureTask) {
  CONCURRENCY_TYPE(TFuture) *future;
  CONCURRENCY_TYPE(FutureFunction) function;
  void *argument;
} CONCURRENCY_TYPE(TFutureTask);

#define FUTURE_TASK_PROTOTYPE                                                 \
  static inline OPSTATUS FUTURE_FUNC(Task)(                                   \
      CONCURRENCY_TYPE(TFutureTask) *futureTask,                              \
      CONCURRENCY_TYPE(TFuture) *future,                                      \
      CONCURRENCY_TYPE(FutureFunction) function, void *argument,              \
      CONCURRENCY_TYPE(TTask) *outTask)

FUTURE_TASK_PROTOTYPE;

#include "Impl/Future.impl"
