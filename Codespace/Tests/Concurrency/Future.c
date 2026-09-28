#include "../../Cosmeron/Modules/Concurrency/Concurrency.h"
#include <assert.h>

static OPSTATUS compute(void *argument, void **result) {
  uint32_t *value = (uint32_t *)argument;
  ++*value;
  *result = value;
  return STATUS_CONST(SUCCESS);
}

int main(void) {
  CONCURRENCY_TYPE(TThread)Pool pool;
  CONCURRENCY_TYPE(TThread) threads[2];
  CONCURRENCY_TYPE(TTask) queue[4];
  CONCURRENCY_TYPE(TFuture) future;
  CONCURRENCY_TYPE(CONCURRENCY_TYPE(TFuture)Task) futureTask;
  uint32_t value = 41;
  void *result = NULL;
  OPSTATUS status = STATUS_CONST(GENERIC_ERROR);

  assert(FUTURE_FUNC(Init)(&future) == STATUS_CONST(SUCCESS));
  assert(THREAD_POOL_FUNC(Init)(&pool, threads, 2, queue, 4) == STATUS_CONST(SUCCESS));
  assert(THREAD_POOL_FUNC(Submit)(
      &pool, FUTURE_FUNC(Task)(&futureTask, &future, compute, &value)) ==
      STATUS_CONST(SUCCESS));
  assert(FUTURE_FUNC(Wait)(&future) == STATUS_CONST(SUCCESS));
  assert(FUTURE_FUNC(Get)(&future, &result, &status) == STATUS_CONST(SUCCESS));
  assert(result == &value);
  assert(value == 42);
  assert(status == STATUS_CONST(SUCCESS));
  assert(FUTURE_FUNC(IsReady)(&future));
  assert(FUTURE_FUNC(Complete)(&future, NULL, STATUS_CONST(GENERIC_ERROR)) ==
         STATUS_CONST(ALREADY_EXISTS));

  assert(THREAD_POOL_FUNC(Destroy)(&pool) == STATUS_CONST(SUCCESS));
  assert(FUTURE_FUNC(Destroy)(&future) == STATUS_CONST(SUCCESS));
  assert(FUTURE_FUNC(Destroy)(&future) == STATUS_CONST(INVALID_ARGUMENT));
  assert(!FUTURE_FUNC(IsReady)(&future));
  assert(FUTURE_FUNC(Wait)(&future) == STATUS_CONST(INVALID_ARGUMENT));
  return 0;
}
