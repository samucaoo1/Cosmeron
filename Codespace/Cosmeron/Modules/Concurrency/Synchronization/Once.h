#pragma once

#include "../Concurrency.space"
#include "../../../Core/Error/Status.h"

typedef void (*TOnceFunction)(void);

#if OS_POSIX
#include <pthread.h>
typedef struct TOnce { pthread_once_t native; } TOnce;
#define ONCE_INIT { PTHREAD_ONCE_INIT }
#elif OS_WINDOWS
#include <windows.h>
typedef struct TOnce {
  INIT_ONCE native;
} TOnce;
#define ONCE_INIT { INIT_ONCE_STATIC_INIT }
#endif

inline static OPSTATUS ONCE_FUNC(Call)(TOnce *once, TOnceFunction function);

#include "Impl/Once.impl"
