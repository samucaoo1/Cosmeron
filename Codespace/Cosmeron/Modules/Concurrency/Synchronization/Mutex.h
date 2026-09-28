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

static inline OPSTATUS MUTEX_FUNC(Init)(CONCURRENCY_TYPE(TMutex) *mutex);
static inline OPSTATUS MUTEX_FUNC(Destroy)(CONCURRENCY_TYPE(TMutex) *mutex);
static inline OPSTATUS MUTEX_FUNC(Lock)(CONCURRENCY_TYPE(TMutex) *mutex);
static inline OPSTATUS MUTEX_FUNC(TryLock)(CONCURRENCY_TYPE(TMutex) *mutex, bool *outAcquired);
static inline OPSTATUS MUTEX_FUNC(Unlock)(CONCURRENCY_TYPE(TMutex) *mutex);

#include "Impl/Mutex.impl"
