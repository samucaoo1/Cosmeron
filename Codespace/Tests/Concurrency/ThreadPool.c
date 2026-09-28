#include "../../Cosmeron/Modules/Concurrency/Concurrency.h"
#include <assert.h>

#define THREAD_COUNT 4
#define QUEUE_CAPACITY 32
#define TASK_COUNT 10000

static void increment(void *argument) {
  CONCURRENCY_TYPE(TAtomicU32) *counter = (CONCURRENCY_TYPE(TAtomicU32) *)argument;
  (void)ATOMIC_FUNC(U32_FetchAdd)(counter, 1, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_RELAXED));
}

int main(void) {
  CONCURRENCY_TYPE(TThread)Pool pool;
  CONCURRENCY_TYPE(TThread) threads[THREAD_COUNT];
  CONCURRENCY_TYPE(TTask) queue[QUEUE_CAPACITY];
  CONCURRENCY_TYPE(TAtomicU32) counter = ATOMIC_U32_INIT(0);

  assert(THREAD_POOL_FUNC(Init)(&pool, threads, THREAD_COUNT,
                              queue, QUEUE_CAPACITY) == STATUS_CONST(SUCCESS));
  for (unsigned i = 0; i < TASK_COUNT; ++i)
    assert(THREAD_POOL_FUNC(Submit)(
        &pool, TASK_FUNC(Create)(increment, &counter)) == STATUS_CONST(SUCCESS));

  assert(THREAD_POOL_FUNC(Destroy)(&pool) == STATUS_CONST(SUCCESS));
  assert(ATOMIC_FUNC(U32_Load)(&counter, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST)) == TASK_COUNT);
  return 0;
}
