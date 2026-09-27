#include <assert.h>
#include "../../Cosmeron/Modules/Concurrency/Atomic/Atomic.h"
#include "../../Cosmeron/Modules/Concurrency/Thread/Thread.h"
#include "../../Cosmeron/Modules/Concurrency/Thread/Sleep.h"

static void worker(void *argument) {
  TAtomicU32 *value = (TAtomicU32 *)argument;
  (void)ATOMIC_FUNC(U32_FetchAdd)(value, 1, CONCURRENCY_ATOMIC_MEMORY_ORDER_SEQ_CST);
}

int main(void) {
  TThread thread;
  TAtomicU32 value = ATOMIC_U32_INIT(0);

  assert(THREAD_FUNC(Equal)(THREAD_FUNC(Current)(), THREAD_FUNC(Current)()));
  THREAD_FUNC(Yield)();

  assert(THREAD_FUNC(Create)(&thread, worker, &value) == STATUS_CONST(SUCCESS));
  assert(THREAD_FUNC(Join)(&thread) == STATUS_CONST(SUCCESS));
  assert(ATOMIC_FUNC(U32_Load)(&value, CONCURRENCY_ATOMIC_MEMORY_ORDER_SEQ_CST) == 1);

  assert(THREAD_FUNC(Create)(&thread, worker, &value) == STATUS_CONST(SUCCESS));
  assert(THREAD_FUNC(Detach)(&thread) == STATUS_CONST(SUCCESS));
  for (unsigned i = 0; i < 1000000U; ++i) {
    if (ATOMIC_FUNC(U32_Load)(&value, CONCURRENCY_ATOMIC_MEMORY_ORDER_SEQ_CST) == 2U)
      break;
    THREAD_FUNC(Yield)();
  }
  assert(ATOMIC_FUNC(U32_Load)(&value, CONCURRENCY_ATOMIC_MEMORY_ORDER_SEQ_CST) == 2U);

  assert(THREAD_FUNC(SleepFor)(
             Chronometry_Duration_FromNanoseconds(0)) == STATUS_CONST(SUCCESS));
  {
    Chronometry_ClockTimePoint now;
    assert(Chronometry_Clock_Now(&now) == STATUS_CONST(SUCCESS));
    assert(THREAD_FUNC(SleepUntil)(now) == STATUS_CONST(SUCCESS));
  }

  return 0;
}
