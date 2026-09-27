#pragma once
#include "../Concurrency.space"
#include "../../../Core/Error/Status.h"
#if OS_POSIX
#include <pthread.h>
typedef struct TRWLock { pthread_rwlock_t native; } TRWLock;
#elif OS_WINDOWS
#include <windows.h>
typedef struct TRWLock { SRWLOCK native; } TRWLock;
#endif
inline static OPSTATUS RWLOCK_FUNC(Init)(TRWLock *lock);
inline static OPSTATUS RWLOCK_FUNC(Destroy)(TRWLock *lock);
inline static OPSTATUS RWLOCK_FUNC(ReadLock)(TRWLock *lock);
inline static OPSTATUS RWLOCK_FUNC(TryReadLock)(TRWLock *lock, bool *acquired);
inline static OPSTATUS RWLOCK_FUNC(ReadUnlock)(TRWLock *lock);
inline static OPSTATUS RWLOCK_FUNC(WriteLock)(TRWLock *lock);
inline static OPSTATUS RWLOCK_FUNC(TryWriteLock)(TRWLock *lock, bool *acquired);
inline static OPSTATUS RWLOCK_FUNC(WriteUnlock)(TRWLock *lock);
#include "Impl/RWLock.impl"
