#include "../../Cosmeron/Modules/Concurrency/Concurrency.h"
#include <assert.h>

#define THREAD_COUNT 8
#define ITERATIONS 100000

typedef struct TContention_Context {
  TAtomicU32 atomic_counter;
  uint32_t mutex_counter;
  TMutex mutex;
} TContention_Context;

static void worker(void *argument) {
  TContention_Context *context = (TContention_Context *)argument;
  for (unsigned i = 0; i < ITERATIONS; ++i) {
    ATOMIC_FUNC(U32_FetchAdd)(&context->atomic_counter, 1, CONCURRENCY_ATOMIC_MEMORY_ORDER_RELAXED);
    MUTEX_FUNC(Lock)(&context->mutex);
    ++context->mutex_counter;
    MUTEX_FUNC(Unlock)(&context->mutex);
  }
  return;
}

int main(void) {
  TContention_Context context = { .atomic_counter = ATOMIC_U32_INIT(0), .mutex_counter = 0 };
  TThread threads[THREAD_COUNT];
  assert(MUTEX_FUNC(Init)(&context.mutex) == STATUS_CONST(SUCCESS));
  for (unsigned i = 0; i < THREAD_COUNT; ++i)
    assert(THREAD_FUNC(Create)(&threads[i], worker, &context) == STATUS_CONST(SUCCESS));
  for (unsigned i = 0; i < THREAD_COUNT; ++i)
    assert(THREAD_FUNC(Join)(&threads[i]) == STATUS_CONST(SUCCESS));
  assert(ATOMIC_FUNC(U32_Load)(&context.atomic_counter, CONCURRENCY_ATOMIC_MEMORY_ORDER_SEQ_CST) ==
         THREAD_COUNT * ITERATIONS);
  assert(context.mutex_counter == THREAD_COUNT * ITERATIONS);
  assert(MUTEX_FUNC(Destroy)(&context.mutex) == STATUS_CONST(SUCCESS));
  return 0;
}
