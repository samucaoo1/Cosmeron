#include "../../Cosmeron/Modules/Concurrency/Concurrency.h"
#include <assert.h>

#define THREAD_COUNT 2
#define QUEUE_CAPACITY 8
#define TASK_COUNT 1000

static void increment(void *argument) {
  TAtomicU32 *counter = (TAtomicU32 *)argument;
  (void)ATOMIC_FUNC(U32_FetchAdd)(counter, 1, CONCURRENCY_ATOMIC_MEMORY_ORDER_RELAXED);
}

int main(void) {
  TThreadPool pool;
  TThread threads[THREAD_COUNT];
  TTask queue[QUEUE_CAPACITY];
  TAtomicU32 counter = ATOMIC_U32_INIT(0);

  assert(THREAD_POOL_FUNC(Init)(&pool, threads, THREAD_COUNT,
                              queue, QUEUE_CAPACITY) == STATUS_CONST(SUCCESS));
  for (unsigned i = 0; i < TASK_COUNT; ++i)
    assert(THREAD_POOL_FUNC(Submit)(
        &pool, TASK_FUNC(Create)(increment, &counter)) == STATUS_CONST(SUCCESS));

  assert(THREAD_POOL_FUNC(Shutdown)(&pool) == STATUS_CONST(SUCCESS));
  {
    bool submitted = true;
    assert(THREAD_POOL_FUNC(TrySubmit)(
               &pool, TASK_FUNC(Create)(increment, &counter), &submitted) ==
           STATUS_CONST(CANCELLED));
    assert(!submitted);
  }
  assert(THREAD_POOL_FUNC(Destroy)(&pool) == STATUS_CONST(SUCCESS));
  assert(THREAD_POOL_FUNC(Destroy)(&pool) == STATUS_CONST(INVALID_ARGUMENT));

  assert(ATOMIC_FUNC(U32_Load)(&counter, CONCURRENCY_ATOMIC_MEMORY_ORDER_SEQ_CST) == TASK_COUNT);
  return 0;
}
