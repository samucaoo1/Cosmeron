#pragma once

#include "../Concurrency.space"
#include "../../../Core/Error/Status.h"

#if OS_POSIX
#include <pthread.h>
typedef pthread_t CONCURRENCY_TYPE(ThreadNative);
#elif OS_WINDOWS
#include <windows.h>
typedef HANDLE CONCURRENCY_TYPE(ThreadNative);
#endif

typedef void (*CONCURRENCY_TYPE(ThreadFunction))(void *);

typedef struct CONCURRENCY_TYPE(TThread) {
  CONCURRENCY_TYPE(ThreadNative) native;
  bool joinable;
} CONCURRENCY_TYPE(TThread);

static inline OPSTATUS THREAD_FUNC(Create)(CONCURRENCY_TYPE(TThread) *thread, CONCURRENCY_TYPE(ThreadFunction) function,
                                     void *argument);
static inline OPSTATUS THREAD_FUNC(Join)(CONCURRENCY_TYPE(TThread) *thread);
static inline OPSTATUS THREAD_FUNC(Detach)(CONCURRENCY_TYPE(TThread) *thread);
static inline void THREAD_FUNC(Yield)(void);
static inline CONCURRENCY_TYPE(ThreadNative) THREAD_FUNC(Current)(void);
static inline bool THREAD_FUNC(Equal)(CONCURRENCY_TYPE(ThreadNative) first,
                                    CONCURRENCY_TYPE(ThreadNative) second);

#include "Impl/Thread.impl"
