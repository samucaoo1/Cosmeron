#include "../../Cosmeron/Modules/Concurrency/Cooperative.h"

typedef struct {
  CONCURRENCY_TYPE(TAtomicU32) accumulator;
  unsigned transitions;
} TCooperativeContext;

static void RunTask(void *argument) {
  TCooperativeContext *ctx = (TCooperativeContext *)argument;
  (void)ATOMIC_FUNC(U32_FetchAdd)(&ctx->accumulator, 2U,
    CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
}

static void RunCoroutine(CONCURRENCY_TYPE(TCoroutine) *coroutine,
                         void *argument) {
  TCooperativeContext *ctx = (TCooperativeContext *)argument;
  COROUTINE_BEGIN(coroutine);
  ++ctx->transitions;
  COROUTINE_YIELD(coroutine);
  ++ctx->transitions;
  COROUTINE_END(coroutine);
}

int main(void) {
  TCooperativeContext ctx = {ATOMIC_U32_INIT(0), 0U};
  CONCURRENCY_TYPE(TCoroutine) coro = COROUTINE_INIT(RunCoroutine, &ctx);
  CONCURRENCY_TYPE(TTask) task = TASK_FUNC(Create)(RunTask, &ctx);
  if (!TASK_FUNC(IsValid)(&task) ||
      TASK_FUNC(Run)(&task) != STATUS_CONST(SUCCESS)) return 1;
  if (COROUTINE_FUNC(Resume)(&coro) != STATUS_CONST(SUCCESS) ||
      ctx.transitions != 1U) return 2;
  if (COROUTINE_FUNC(Resume)(&coro) != STATUS_CONST(SUCCESS) ||
      ctx.transitions != 2U) return 3;
  if (ATOMIC_FUNC(U32_Load)(&ctx.accumulator,
      CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST)) != 2U) return 4;
  return 0;
}
