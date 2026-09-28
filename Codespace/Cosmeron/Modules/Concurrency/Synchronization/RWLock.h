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
#define RWLOCK_INIT_PROTOTYPE                                                 \
  static inline OPSTATUS RWLOCK_FUNC(Init)(CONCURRENCY_TYPE(TRWLock) *lock)
#define RWLOCK_DESTROY_PROTOTYPE                                              \
  static inline OPSTATUS RWLOCK_FUNC(Destroy)(CONCURRENCY_TYPE(TRWLock) *lock)
#define RWLOCK_READ_LOCK_PROTOTYPE                                            \
  static inline OPSTATUS RWLOCK_FUNC(ReadLock)(CONCURRENCY_TYPE(TRWLock) *lock)
#define RWLOCK_TRY_READ_LOCK_PROTOTYPE                                        \
  static inline OPSTATUS RWLOCK_FUNC(TryReadLock)(                            \
      CONCURRENCY_TYPE(TRWLock) *lock, bool *outAcquired)
#define RWLOCK_READ_UNLOCK_PROTOTYPE                                          \
  static inline OPSTATUS RWLOCK_FUNC(ReadUnlock)(CONCURRENCY_TYPE(TRWLock) *lock)
#define RWLOCK_WRITE_LOCK_PROTOTYPE                                           \
  static inline OPSTATUS RWLOCK_FUNC(WriteLock)(CONCURRENCY_TYPE(TRWLock) *lock)
#define RWLOCK_TRY_WRITE_LOCK_PROTOTYPE                                       \
  static inline OPSTATUS RWLOCK_FUNC(TryWriteLock)(                           \
      CONCURRENCY_TYPE(TRWLock) *lock, bool *outAcquired)
#define RWLOCK_WRITE_UNLOCK_PROTOTYPE                                         \
  static inline OPSTATUS RWLOCK_FUNC(WriteUnlock)(CONCURRENCY_TYPE(TRWLock) *lock)

RWLOCK_INIT_PROTOTYPE;
RWLOCK_DESTROY_PROTOTYPE;
RWLOCK_READ_LOCK_PROTOTYPE;
RWLOCK_TRY_READ_LOCK_PROTOTYPE;
RWLOCK_READ_UNLOCK_PROTOTYPE;
RWLOCK_WRITE_LOCK_PROTOTYPE;
RWLOCK_TRY_WRITE_LOCK_PROTOTYPE;
RWLOCK_WRITE_UNLOCK_PROTOTYPE;
#include "Impl/RWLock.impl"
