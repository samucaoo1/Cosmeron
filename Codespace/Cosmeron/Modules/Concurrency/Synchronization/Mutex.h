#pragma once

#include "../Concurrency.space"
#include "../../../Core/Error/Status.h"

#if OS_POSIX
#include <pthread.h>
typedef struct CONCURRENCY_TYPE(TMutex) { pthread_mutex_t native; } CONCURRENCY_TYPE(TMutex);
#elif OS_WINDOWS
#include <windows.h>
typedef struct CONCURRENCY_TYPE(TMutex) { SRWLOCK native; } CONCURRENCY_TYPE(TMutex);
#endif

#define MUTEX_INIT_PROTOTYPE                                                  \
  static inline OPSTATUS MUTEX_FUNC(Init)(CONCURRENCY_TYPE(TMutex) *mutex)
#define MUTEX_DESTROY_PROTOTYPE                                               \
  static inline OPSTATUS MUTEX_FUNC(Destroy)(CONCURRENCY_TYPE(TMutex) *mutex)
#define MUTEX_LOCK_PROTOTYPE                                                  \
  static inline OPSTATUS MUTEX_FUNC(Lock)(CONCURRENCY_TYPE(TMutex) *mutex)
#define MUTEX_TRY_LOCK_PROTOTYPE                                              \
  static inline OPSTATUS MUTEX_FUNC(TryLock)(CONCURRENCY_TYPE(TMutex) *mutex, \
                                              bool *outAcquired)
#define MUTEX_UNLOCK_PROTOTYPE                                                \
  static inline OPSTATUS MUTEX_FUNC(Unlock)(CONCURRENCY_TYPE(TMutex) *mutex)

MUTEX_INIT_PROTOTYPE;
MUTEX_DESTROY_PROTOTYPE;
MUTEX_LOCK_PROTOTYPE;
MUTEX_TRY_LOCK_PROTOTYPE;
MUTEX_UNLOCK_PROTOTYPE;

#include "Impl/Mutex.impl"
