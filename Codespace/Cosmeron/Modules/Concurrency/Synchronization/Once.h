#pragma once

#include "../Concurrency.space"
#include "../../../Core/Error/Status.h"

typedef void (*CONCURRENCY_TYPE(OnceFunction))(void);

#if OS_POSIX
#include <pthread.h>
typedef struct CONCURRENCY_TYPE(TOnce) { pthread_once_t native; } CONCURRENCY_TYPE(TOnce);
#define ONCE_INIT { PTHREAD_ONCE_INIT }
#elif OS_WINDOWS
#include <windows.h>
typedef struct CONCURRENCY_TYPE(TOnce) {
  INIT_ONCE native;
} CONCURRENCY_TYPE(TOnce);
#define ONCE_INIT { INIT_ONCE_STATIC_INIT }
#endif

#define ONCE_CALL_PROTOTYPE                                                   \
  static inline OPSTATUS ONCE_FUNC(Call)(                                     \
      CONCURRENCY_TYPE(TOnce) *once, CONCURRENCY_TYPE(OnceFunction) function)

ONCE_CALL_PROTOTYPE;

#include "Impl/Once.impl"
