#pragma once
#include "../Concurrency.space"
#include "../../../Core/Error/Status.h"
#if OS_POSIX
#include <pthread.h>
typedef struct CONCURRENCY_TYPE(TRWLock) { pthread_rwlock_t native; } CONCURRENCY_TYPE(TRWLock);
#elif OS_WINDOWS
#include <windows.h>
typedef struct CONCURRENCY_TYPE(TRWLock) { SRWLOCK native; } CONCURRENCY_TYPE(TRWLock);
#endif
static inline OPSTATUS RWLOCK_FUNC(Init)(CONCURRENCY_TYPE(TRWLock) *lock);
static inline OPSTATUS RWLOCK_FUNC(Destroy)(CONCURRENCY_TYPE(TRWLock) *lock);
static inline OPSTATUS RWLOCK_FUNC(ReadLock)(CONCURRENCY_TYPE(TRWLock) *lock);
static inline OPSTATUS RWLOCK_FUNC(TryReadLock)(CONCURRENCY_TYPE(TRWLock) *lock, bool *outAcquired);
static inline OPSTATUS RWLOCK_FUNC(ReadUnlock)(CONCURRENCY_TYPE(TRWLock) *lock);
static inline OPSTATUS RWLOCK_FUNC(WriteLock)(CONCURRENCY_TYPE(TRWLock) *lock);
static inline OPSTATUS RWLOCK_FUNC(TryWriteLock)(CONCURRENCY_TYPE(TRWLock) *lock, bool *outAcquired);
static inline OPSTATUS RWLOCK_FUNC(WriteUnlock)(CONCURRENCY_TYPE(TRWLock) *lock);
#include "Impl/RWLock.impl"
