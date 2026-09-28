#include "../../Cosmeron/Modules/Concurrency/Concurrency.h"

#include <assert.h>

typedef struct CONCURRENCY_TYPE(TCoroutine)_Test_Context {
  unsigned step;
  unsigned value;
} CONCURRENCY_TYPE(TCoroutine)_Test_Context;

static void Test_Coroutine(CONCURRENCY_TYPE(TCoroutine) *coroutine, void *argument) {
  CONCURRENCY_TYPE(TCoroutine)_Test_Context *context = argument;

  COROUTINE_BEGIN(coroutine);

  assert(COROUTINE_FUNC(IsRunning)(coroutine));
  context->step = 1u;
  context->value += 10u;
  COROUTINE_YIELD(coroutine);

  context->step = 2u;
  context->value += 20u;
  COROUTINE_YIELD(coroutine);

  context->step = 3u;
  context->value += 30u;

  COROUTINE_END(coroutine);
}

int main(void) {
  CONCURRENCY_TYPE(TCoroutine)_Test_Context context = {0u, 0u};
  CONCURRENCY_TYPE(TCoroutine) coroutine = COROUTINE_INIT(Test_Coroutine, &context);

  assert(COROUTINE_FUNC(GetState)(&coroutine) == CONCURRENCY_CONST(COROUTINE, STATE_READY));
  assert(COROUTINE_FUNC(IsReady)(&coroutine));
  assert(COROUTINE_FUNC(Resume)(&coroutine) == STATUS_CONST(SUCCESS));
  assert(context.step == 1u && context.value == 10u);
  assert(COROUTINE_FUNC(GetState)(&coroutine) == CONCURRENCY_CONST(COROUTINE, STATE_SUSPENDED));
  assert(COROUTINE_FUNC(IsSuspended)(&coroutine));

  assert(COROUTINE_FUNC(Resume)(&coroutine) == STATUS_CONST(SUCCESS));
  assert(context.step == 2u && context.value == 30u);
  assert(COROUTINE_FUNC(IsSuspended)(&coroutine));

  assert(COROUTINE_FUNC(Resume)(&coroutine) == STATUS_CONST(SUCCESS));
  assert(context.step == 3u && context.value == 60u);
  assert(COROUTINE_FUNC(GetState)(&coroutine) == CONCURRENCY_CONST(COROUTINE, STATE_FINISHED));
  assert(COROUTINE_FUNC(IsFinished)(&coroutine));
  assert(COROUTINE_FUNC(Resume)(&coroutine) == STATUS_CONST(NOT_AVAILABLE));

  assert(COROUTINE_FUNC(Reset)(&coroutine) == STATUS_CONST(SUCCESS));
  assert(COROUTINE_FUNC(IsReady)(&coroutine));
  assert(COROUTINE_FUNC(Resume)(&coroutine) == STATUS_CONST(SUCCESS));
  assert(context.step == 1u && context.value == 70u);

  return 0;
}
