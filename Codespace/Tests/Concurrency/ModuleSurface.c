#include "../../Cosmeron/Modules/Concurrency/Concurrency.h"

static void CoroutineBody(CONCURRENCY_TYPE(TCoroutine) *coroutine, void *argument) {
  unsigned *steps = (unsigned *)argument;
  COROUTINE_BEGIN(coroutine);
  ++*steps;
  COROUTINE_YIELD(coroutine);
  ++*steps;
  COROUTINE_END(coroutine);
}

static OPSTATUS FutureBody(void *argument, void **result) {
  *result = argument;
  return STATUS_CONST(SUCCESS);
}

static void TaskBody(void *argument) {
  CONCURRENCY_TYPE(TAtomicU32) *value = (CONCURRENCY_TYPE(TAtomicU32) *)argument;
  (void)ATOMIC_FUNC(U32_FetchAdd)(value, 1U,
                                  CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
}

static void ThreadBody(void *argument) {
  CONCURRENCY_TYPE(TAtomicU32) *value = (CONCURRENCY_TYPE(TAtomicU32) *)argument;
  (void)ATOMIC_FUNC(U32_FetchAdd)(value, 1U,
                                  CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
}

int main(void) {
  unsigned steps = 0U;
  CONCURRENCY_TYPE(TCoroutine) coroutine;
  CONCURRENCY_TYPE(TAtomicU32) value = ATOMIC_U32_INIT(0U);
  CONCURRENCY_TYPE(TThread) thread;
  CONCURRENCY_TYPE(TSemaphore) semaphore;
  CONCURRENCY_TYPE(TBarrier) barrier;
  CONCURRENCY_TYPE(TFuture) future;
  CONCURRENCY_TYPE(TFutureTask) futureTask;
  CONCURRENCY_TYPE(TTask) queue[2];
  CONCURRENCY_TYPE(TThread) workers[1];
  CONCURRENCY_TYPE(TThreadPool) pool;
  void *futureResult = NULL;
  OPSTATUS futureStatus = STATUS_CONST(GENERIC_ERROR);
  bool acquired = false;
  bool submitted = false;

  if (COROUTINE_FUNC(Init)(&coroutine, CoroutineBody, &steps) != STATUS_CONST(SUCCESS))
    return 1;
  if (COROUTINE_FUNC(Resume)(&coroutine) != STATUS_CONST(SUCCESS) || steps != 1U)
    return 2;
  if (COROUTINE_FUNC(Resume)(&coroutine) != STATUS_CONST(SUCCESS) || steps != 2U)
    return 3;

  if (THREAD_FUNC(Create)(&thread, ThreadBody, &value) != STATUS_CONST(SUCCESS))
    return 4;
  if (THREAD_FUNC(Join)(&thread) != STATUS_CONST(SUCCESS))
    return 5;
  if (ATOMIC_FUNC(U32_Load)(&value, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST)) != 1U)
    return 6;

  if (SEMAPHORE_FUNC(Init)(&semaphore, 1U) != STATUS_CONST(SUCCESS))
    return 7;
  if (SEMAPHORE_FUNC(TryWait)(&semaphore, &acquired) != STATUS_CONST(SUCCESS) ||
      !acquired)
    return 8;
  if (SEMAPHORE_FUNC(Post)(&semaphore) != STATUS_CONST(SUCCESS))
    return 9;
  if (SEMAPHORE_FUNC(Destroy)(&semaphore) != STATUS_CONST(SUCCESS))
    return 10;

  if (BARRIER_FUNC(Init)(&barrier, 1U) != STATUS_CONST(SUCCESS))
    return 11;
  if (BARRIER_FUNC(Wait)(&barrier) != STATUS_CONST(SUCCESS))
    return 12;
  if (BARRIER_FUNC(Destroy)(&barrier) != STATUS_CONST(SUCCESS))
    return 13;

  if (FUTURE_FUNC(Init)(&future) != STATUS_CONST(SUCCESS))
    return 14;
  CONCURRENCY_TYPE(TTask) futureTaskValue;
  if (FUTURE_FUNC(Task)(&futureTask, &future, FutureBody, &value,
                        &futureTaskValue) != STATUS_CONST(SUCCESS))
    return 15;
  if (!TASK_FUNC(IsValid)(&futureTaskValue))
    return 27;
  if (TASK_FUNC(Run)(&futureTaskValue) != STATUS_CONST(SUCCESS))
    return 16;
  if (FUTURE_FUNC(Get)(&future, &futureResult, &futureStatus) !=
      STATUS_CONST(SUCCESS))
    return 17;
  if (futureResult != &value || futureStatus != STATUS_CONST(SUCCESS))
    return 18;
  if (FUTURE_FUNC(Destroy)(&future) != STATUS_CONST(SUCCESS))
    return 19;

  if (THREAD_POOL_FUNC(Init)(&pool, workers, 1U, queue, 2U) !=
      STATUS_CONST(SUCCESS))
    return 20;
  {
    CONCURRENCY_TYPE(TTask) task = TASK_FUNC(Create)(TaskBody, &value);
    if (!TASK_FUNC(IsValid)(&task))
      return 21;
    if (THREAD_POOL_FUNC(TrySubmit)(&pool, task, &submitted) !=
            STATUS_CONST(SUCCESS) ||
        !submitted)
      return 22;
    if (THREAD_POOL_FUNC(Submit)(&pool, task) != STATUS_CONST(SUCCESS))
      return 23;
  }
  if (THREAD_POOL_FUNC(Shutdown)(&pool) != STATUS_CONST(SUCCESS))
    return 24;
  if (THREAD_POOL_FUNC(Destroy)(&pool) != STATUS_CONST(SUCCESS))
    return 25;
  if (ATOMIC_FUNC(U32_Load)(&value, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST)) != 3U)
    return 26;

  return 0;
}
