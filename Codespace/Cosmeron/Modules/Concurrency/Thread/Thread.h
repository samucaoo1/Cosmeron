#pragma once

#include "../Concurrency.space"
#include "../../../Core/Error/Status.h"

#if OS_POSIX
#include <pthread.h>
typedef pthread_t TThreadNative;
#elif OS_WINDOWS
#include <windows.h>
typedef HANDLE TThreadNative;
#endif

typedef void (*TThreadFunction)(void *);

typedef struct TThread {
  TThreadNative native;
  bool joinable;
} TThread;

inline static OPSTATUS THREAD_FUNC(Create)(TThread *thread, TThreadFunction function,
                                     void *argument);
inline static OPSTATUS THREAD_FUNC(Join)(TThread *thread);
inline static OPSTATUS THREAD_FUNC(Detach)(TThread *thread);
inline static void THREAD_FUNC(Yield)(void);
inline static TThreadNative THREAD_FUNC(Current)(void);
inline static bool THREAD_FUNC(Equal)(TThreadNative first,
                                    TThreadNative second);

#include "Impl/Thread.impl"
