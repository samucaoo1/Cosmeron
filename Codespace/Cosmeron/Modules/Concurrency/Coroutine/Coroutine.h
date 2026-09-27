#pragma once

#include "../Concurrency.space"
#include "../../../Core/Error/Status.h"

typedef enum TCoroutineState {
  CONCURRENCY_COROUTINE_STATE_READY = 0,
  CONCURRENCY_COROUTINE_STATE_RUNNING,
  CONCURRENCY_COROUTINE_STATE_SUSPENDED,
  CONCURRENCY_COROUTINE_STATE_FINISHED
} TCoroutineState;

typedef struct TCoroutine TCoroutine;
typedef void (*TCoroutineFunction)(TCoroutine *coroutine, void *argument);

struct TCoroutine {
  TCoroutineFunction function;
  void *argument;
  uint32_t continuation;
  TCoroutineState state;
};

#define COROUTINE_INIT(FUNCTION, ARGUMENT)                                      \
  { (FUNCTION), (ARGUMENT), 0u, CONCURRENCY_COROUTINE_STATE_READY }

#define COROUTINE_BEGIN(COROUTINE)                                              \
  do {                                                                          \
    TCoroutine *_coroutine = (COROUTINE);                                       \
    if (_coroutine == NULL ||                                                   \
        _coroutine->state == CONCURRENCY_COROUTINE_STATE_FINISHED)                          \
      return;                                                                   \
    _coroutine->state = CONCURRENCY_COROUTINE_STATE_RUNNING;                                \
    switch (_coroutine->continuation) {                                         \
    case 0u:

#define COROUTINE_YIELD(COROUTINE)                                              \
  do {                                                                          \
    (COROUTINE)->continuation = (uint32_t)__LINE__;                             \
    (COROUTINE)->state = CONCURRENCY_COROUTINE_STATE_SUSPENDED;                             \
    return;                                                                     \
  case __LINE__:;                                                               \
  } while (0)

#define COROUTINE_END(COROUTINE)                                                \
    default:                                                                    \
      break;                                                                    \
    }                                                                           \
    (COROUTINE)->continuation = 0u;                                             \
    (COROUTINE)->state = CONCURRENCY_COROUTINE_STATE_FINISHED;                              \
    return;                                                                     \
  } while (0)

inline static OPSTATUS COROUTINE_FUNC(Init)(TCoroutine *coroutine,
                                      TCoroutineFunction function,
                                      void *argument);
inline static OPSTATUS COROUTINE_FUNC(Resume)(TCoroutine *coroutine);
inline static OPSTATUS COROUTINE_FUNC(Reset)(TCoroutine *coroutine);
inline static TCoroutineState
COROUTINE_FUNC(GetState)(const TCoroutine *coroutine);
inline static bool COROUTINE_FUNC(IsReady)(const TCoroutine *coroutine);
inline static bool COROUTINE_FUNC(IsRunning)(const TCoroutine *coroutine);
inline static bool COROUTINE_FUNC(IsSuspended)(const TCoroutine *coroutine);
inline static bool COROUTINE_FUNC(IsFinished)(const TCoroutine *coroutine);

#include "Impl/Coroutine.impl"

/* EOF */
