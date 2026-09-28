#pragma once

#include "../Concurrency.space"
#include "../../../Core/Error/Status.h"

typedef enum CONCURRENCY_TYPE(CoroutineState) {
  CONCURRENCY_CONST(COROUTINE, STATE_READY) = 0,
  CONCURRENCY_CONST(COROUTINE, STATE_RUNNING),
  CONCURRENCY_CONST(COROUTINE, STATE_SUSPENDED),
  CONCURRENCY_CONST(COROUTINE, STATE_FINISHED)
} CONCURRENCY_TYPE(CoroutineState);

typedef struct CONCURRENCY_TYPE(TCoroutine) CONCURRENCY_TYPE(TCoroutine);
typedef void (*CONCURRENCY_TYPE(CoroutineFunction))(CONCURRENCY_TYPE(TCoroutine) *coroutine, void *argument);

struct CONCURRENCY_TYPE(TCoroutine) {
  CONCURRENCY_TYPE(CoroutineFunction) function;
  void *argument;
  uint32_t continuation;
  CONCURRENCY_TYPE(CoroutineState) state;
};

#define COROUTINE_INIT(FUNCTION, ARGUMENT)                                      \
  { (FUNCTION), (ARGUMENT), 0u, CONCURRENCY_CONST(COROUTINE, STATE_READY) }

#define COROUTINE_BEGIN(SELF)                                              \
  do {                                                                          \
    CONCURRENCY_TYPE(TCoroutine) *_coroutine = (SELF);                                       \
    if (_coroutine == NULL ||                                                   \
        _coroutine->state == CONCURRENCY_CONST(COROUTINE, STATE_FINISHED))                          \
      return;                                                                   \
    _coroutine->state = CONCURRENCY_CONST(COROUTINE, STATE_RUNNING);                                \
    switch (_coroutine->continuation) {                                         \
    case 0u:

#define COROUTINE_YIELD(SELF)                                              \
  do {                                                                          \
    (SELF)->continuation = (uint32_t)__LINE__;                             \
    (SELF)->state = CONCURRENCY_CONST(COROUTINE, STATE_SUSPENDED);                             \
    return;                                                                     \
  case __LINE__:;                                                               \
  } while (0)

#define COROUTINE_END(SELF)                                                \
    default:                                                                    \
      break;                                                                    \
    }                                                                           \
    (SELF)->continuation = 0u;                                             \
    (SELF)->state = CONCURRENCY_CONST(COROUTINE, STATE_FINISHED);                              \
    return;                                                                     \
  } while (0)

static inline OPSTATUS COROUTINE_FUNC(Init)(CONCURRENCY_TYPE(TCoroutine) *coroutine,
                                      CONCURRENCY_TYPE(CoroutineFunction) function,
                                      void *argument);
static inline OPSTATUS COROUTINE_FUNC(Resume)(CONCURRENCY_TYPE(TCoroutine) *coroutine);
static inline OPSTATUS COROUTINE_FUNC(Reset)(CONCURRENCY_TYPE(TCoroutine) *coroutine);
static inline CONCURRENCY_TYPE(CoroutineState)
COROUTINE_FUNC(GetState)(const CONCURRENCY_TYPE(TCoroutine) *coroutine);
static inline bool COROUTINE_FUNC(IsReady)(const CONCURRENCY_TYPE(TCoroutine) *coroutine);
static inline bool COROUTINE_FUNC(IsRunning)(const CONCURRENCY_TYPE(TCoroutine) *coroutine);
static inline bool COROUTINE_FUNC(IsSuspended)(const CONCURRENCY_TYPE(TCoroutine) *coroutine);
static inline bool COROUTINE_FUNC(IsFinished)(const CONCURRENCY_TYPE(TCoroutine) *coroutine);

#include "Impl/Coroutine.impl"

/* EOF */
