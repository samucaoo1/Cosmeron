#pragma once

#include "../Concurrency.space"
#include "../../../Core/Error/Status.h"

#if OS_POSIX
#include <pthread.h>
typedef struct TMutex { pthread_mutex_t native; } TMutex;
#elif OS_WINDOWS
#include <windows.h>
typedef struct TMutex { SRWLOCK native; } TMutex;
#endif

inline static OPSTATUS MUTEX_FUNC(Init)(TMutex *mutex);
inline static OPSTATUS MUTEX_FUNC(Destroy)(TMutex *mutex);
inline static OPSTATUS MUTEX_FUNC(Lock)(TMutex *mutex);
inline static OPSTATUS MUTEX_FUNC(TryLock)(TMutex *mutex, bool *acquired);
inline static OPSTATUS MUTEX_FUNC(Unlock)(TMutex *mutex);

#include "Impl/Mutex.impl"
