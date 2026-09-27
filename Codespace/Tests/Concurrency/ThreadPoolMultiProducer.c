#include "../../Cosmeron/Modules/Concurrency/Concurrency.h"
#include <assert.h>

#define WORKER_COUNT 4
#define PRODUCER_COUNT 4
#define QUEUE_CAPACITY 16
#define TASKS_PER_PRODUCER 5000

typedef struct TProducer_Context {
  TThreadPool *pool;
  TAtomicU32 *counter;
} TProducer_Context;

static void increment(void *argument) {
  TAtomicU32 *counter = (TAtomicU32 *)argument;
  (void)ATOMIC_FUNC(U32_FetchAdd)(counter, 1, CONCURRENCY_ATOMIC_MEMORY_ORDER_RELAXED);
}

static void producer(void *argument) {
  TProducer_Context *context = (TProducer_Context *)argument;
  for (unsigned i = 0; i < TASKS_PER_PRODUCER; ++i)
    assert(THREAD_POOL_FUNC(Submit)(
        context->pool, TASK_FUNC(Create)(increment, context->counter)) == STATUS_CONST(SUCCESS));
  return;
}

int main(void) {
  TThreadPool pool;
  TThread workers[WORKER_COUNT];
  TThread producers[PRODUCER_COUNT];
  TTask queue[QUEUE_CAPACITY];
  TAtomicU32 counter = ATOMIC_U32_INIT(0);
  TProducer_Context context = { &pool, &counter };

  assert(THREAD_POOL_FUNC(Init)(&pool, workers, WORKER_COUNT,
                              queue, QUEUE_CAPACITY) == STATUS_CONST(SUCCESS));
  for (unsigned i = 0; i < PRODUCER_COUNT; ++i)
    assert(THREAD_FUNC(Create)(&producers[i], producer, &context) == STATUS_CONST(SUCCESS));
  for (unsigned i = 0; i < PRODUCER_COUNT; ++i)
    assert(THREAD_FUNC(Join)(&producers[i]) == STATUS_CONST(SUCCESS));

  assert(THREAD_POOL_FUNC(Destroy)(&pool) == STATUS_CONST(SUCCESS));
  assert(ATOMIC_FUNC(U32_Load)(&counter, CONCURRENCY_ATOMIC_MEMORY_ORDER_SEQ_CST) ==
         PRODUCER_COUNT * TASKS_PER_PRODUCER);
  return 0;
}
