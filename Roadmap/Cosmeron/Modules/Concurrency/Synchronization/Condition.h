#pragma once
#include "Mutex.h"
#if OS_POSIX
#include <pthread.h>
typedef struct CONCURRENCY_TYPE(TCondition) { pthread_cond_t native; } CONCURRENCY_TYPE(TCondition);
#elif OS_WINDOWS
#include <windows.h>
typedef struct CONCURRENCY_TYPE(TCondition) { CONDITION_VARIABLE native; } CONCURRENCY_TYPE(TCondition);
#endif
#define CONDITION_INIT_PROTOTYPE                                              \
  static inline OPSTATUS CONDITION_FUNC(Init)(                                \
      CONCURRENCY_TYPE(TCondition) *condition)
#define CONDITION_DESTROY_PROTOTYPE                                           \
  static inline OPSTATUS CONDITION_FUNC(Destroy)(                             \
      CONCURRENCY_TYPE(TCondition) *condition)
#define CONDITION_WAIT_PROTOTYPE                                              \
  static inline OPSTATUS CONDITION_FUNC(Wait)(                                \
      CONCURRENCY_TYPE(TCondition) *condition, CONCURRENCY_TYPE(TMutex) *mutex)
#define CONDITION_NOTIFY_ONE_PROTOTYPE                                        \
  static inline OPSTATUS CONDITION_FUNC(NotifyOne)(                           \
      CONCURRENCY_TYPE(TCondition) *condition)
#define CONDITION_NOTIFY_ALL_PROTOTYPE                                        \
  static inline OPSTATUS CONDITION_FUNC(NotifyAll)(                           \
      CONCURRENCY_TYPE(TCondition) *condition)

CONDITION_INIT_PROTOTYPE;
CONDITION_DESTROY_PROTOTYPE;
CONDITION_WAIT_PROTOTYPE;
CONDITION_NOTIFY_ONE_PROTOTYPE;
CONDITION_NOTIFY_ALL_PROTOTYPE;
#include "Impl/Condition.impl"
