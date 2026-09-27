#pragma once
#include "Mutex.h"
#if OS_POSIX
#include <pthread.h>
typedef struct TCondition { pthread_cond_t native; } TCondition;
#elif OS_WINDOWS
#include <windows.h>
typedef struct TCondition { CONDITION_VARIABLE native; } TCondition;
#endif
inline static OPSTATUS CONDITION_FUNC(Init)(TCondition *condition);
inline static OPSTATUS CONDITION_FUNC(Destroy)(TCondition *condition);
inline static OPSTATUS CONDITION_FUNC(Wait)(TCondition *condition, TMutex *mutex);
inline static OPSTATUS CONDITION_FUNC(NotifyOne)(TCondition *condition);
inline static OPSTATUS CONDITION_FUNC(NotifyAll)(TCondition *condition);
#include "Impl/Condition.impl"
