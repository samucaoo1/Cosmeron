#include "../../Cosmeron/Modules/Concurrency/Concurrency.h"
#include <assert.h>
#define THREAD_COUNT 4
#define ROUNDS 32
typedef struct { TBarrier *barrier; TAtomicU32 *counter; } TContext;
static void worker(void *argument) {
  TContext *context = (TContext *)argument;
  for (unsigned round = 0; round < ROUNDS; ++round) {
    ATOMIC_FUNC(U32_FetchAdd)(context->counter, 1, CONCURRENCY_ATOMIC_MEMORY_ORDER_RELAXED);
    assert(BARRIER_FUNC(Wait)(context->barrier) == STATUS_CONST(SUCCESS));
    assert(ATOMIC_FUNC(U32_Load)(context->counter, CONCURRENCY_ATOMIC_MEMORY_ORDER_ACQUIRE) >=
           (round + 1) * THREAD_COUNT);
    assert(BARRIER_FUNC(Wait)(context->barrier) == STATUS_CONST(SUCCESS));
  }
  return;
}
int main(void) {
  TBarrier barrier; TThread threads[THREAD_COUNT];
  TAtomicU32 counter = ATOMIC_U32_INIT(0);
  TContext context = { &barrier, &counter };
  assert(BARRIER_FUNC(Init)(&barrier, THREAD_COUNT) == STATUS_CONST(SUCCESS));
  for (unsigned i = 0; i < THREAD_COUNT; ++i)
    assert(THREAD_FUNC(Create)(&threads[i], worker, &context) == STATUS_CONST(SUCCESS));
  for (unsigned i = 0; i < THREAD_COUNT; ++i)
    assert(THREAD_FUNC(Join)(&threads[i]) == STATUS_CONST(SUCCESS));
  assert(ATOMIC_FUNC(U32_Load)(&counter, CONCURRENCY_ATOMIC_MEMORY_ORDER_SEQ_CST) ==
         THREAD_COUNT * ROUNDS);
  assert(BARRIER_FUNC(Destroy)(&barrier) == STATUS_CONST(SUCCESS));
  return 0;
}
