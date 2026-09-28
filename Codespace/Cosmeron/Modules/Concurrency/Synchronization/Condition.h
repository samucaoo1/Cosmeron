#pragma once
#include "Mutex.h"
#if OS_POSIX
#include <pthread.h>
typedef struct CONCURRENCY_TYPE(TCondition) { pthread_cond_t native; } CONCURRENCY_TYPE(TCondition);
#elif OS_WINDOWS
#include <windows.h>
typedef struct CONCURRENCY_TYPE(TCondition) { CONDITION_VARIABLE native; } CONCURRENCY_TYPE(TCondition);
#endif
static inline OPSTATUS CONDITION_FUNC(Init)(CONCURRENCY_TYPE(TCondition) *condition);
static inline OPSTATUS CONDITION_FUNC(Destroy)(CONCURRENCY_TYPE(TCondition) *condition);
static inline OPSTATUS CONDITION_FUNC(Wait)(CONCURRENCY_TYPE(TCondition) *condition, CONCURRENCY_TYPE(TMutex) *mutex);
static inline OPSTATUS CONDITION_FUNC(NotifyOne)(CONCURRENCY_TYPE(TCondition) *condition);
static inline OPSTATUS CONDITION_FUNC(NotifyAll)(CONCURRENCY_TYPE(TCondition) *condition);
#include "Impl/Condition.impl"
