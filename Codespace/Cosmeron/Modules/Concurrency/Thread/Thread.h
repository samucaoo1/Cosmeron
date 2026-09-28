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

#define THREAD_CREATE_PROTOTYPE                                               \
  static inline OPSTATUS THREAD_FUNC(Create)(                                 \
      CONCURRENCY_TYPE(TThread) *thread,                                      \
      CONCURRENCY_TYPE(ThreadFunction) function, void *argument)
#define THREAD_JOIN_PROTOTYPE                                                 \
  static inline OPSTATUS THREAD_FUNC(Join)(CONCURRENCY_TYPE(TThread) *thread)
#define THREAD_DETACH_PROTOTYPE                                               \
  static inline OPSTATUS THREAD_FUNC(Detach)(CONCURRENCY_TYPE(TThread) *thread)
#define THREAD_YIELD_PROTOTYPE                                                \
  static inline void THREAD_FUNC(Yield)(void)
#define THREAD_CURRENT_PROTOTYPE                                              \
  static inline CONCURRENCY_TYPE(ThreadNative) THREAD_FUNC(Current)(void)
#define THREAD_EQUAL_PROTOTYPE                                                \
  static inline bool THREAD_FUNC(Equal)(CONCURRENCY_TYPE(ThreadNative) first, \
                                        CONCURRENCY_TYPE(ThreadNative) second)

THREAD_CREATE_PROTOTYPE;
THREAD_JOIN_PROTOTYPE;
THREAD_DETACH_PROTOTYPE;
THREAD_YIELD_PROTOTYPE;
THREAD_CURRENT_PROTOTYPE;
THREAD_EQUAL_PROTOTYPE;

#include "Impl/Thread.impl"
