#include "../../Cosmeron/Modules/Concurrency/Task/Executor.h"

typedef struct {
  CONCURRENCY_TYPE(TTask) queued;
  int submissions;
} TQueue;
static void Count(void *arg) {
  int *counter = (int *)arg;
  ++*counter;
}
static OPSTATUS Schedule(void *user, CONCURRENCY_TYPE(TTask) task) {
  TQueue *queue = (TQueue *)user;
  if (queue->submissions != 0) return STATUS_CONST(BUSY);
  queue->queued = task; /* executor owns copy, not caller's stack object */
  ++queue->submissions;
  return STATUS_CONST(SUCCESS);
}
int main(void) {
  TQueue queue = {{0},0};
  CONCURRENCY_TYPE(TExecutor) executor = {Schedule,&queue};
  int counter = 0;
  CONCURRENCY_TYPE(TTask) task = TASK_FUNC(Create)(Count,&counter);
  if (CONCURRENCY_FUNC(Executor, Submit)(&executor,task) !=
      STATUS_CONST(SUCCESS) || queue.submissions != 1 || counter != 0)
    return 1;
  if (CONCURRENCY_FUNC(Executor, RunInline)(queue.queued) !=
      STATUS_CONST(SUCCESS) || counter != 1)
    return 2;
  if (CONCURRENCY_FUNC(Executor, Submit)(NULL,task) !=
      STATUS_CONST(INVALID_ARGUMENT))
    return 3;
  return 0;
}
