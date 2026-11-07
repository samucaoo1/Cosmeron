#pragma once
/*
 * External task executor adapter. The host controls threads, lifetimes and
 * scheduling. No implicit thread creation, malloc, locks or link flags.
 *
 * Submit callback MUST copy the TTask pair if execution is deferred.
 * The caller owns argument memory until execution has completed and must
 * provide synchronization for shared state when using a concurrent executor.
 */
#include "../Concurrency.space"
#include "../Task/Task.h"

typedef OPSTATUS (*CONCURRENCY_TYPE(ExecutorSubmit))(void *userData,
    CONCURRENCY_TYPE(TTask) task);
typedef struct CONCURRENCY_TYPE(TExecutor) {
  CONCURRENCY_TYPE(ExecutorSubmit) submit;
  void *userData;
} CONCURRENCY_TYPE(TExecutor);

#define EXECUTOR_SUBMIT_PROTOTYPE                                              \
  static inline OPSTATUS CONCURRENCY_FUNC(Executor, Submit)(                  \
      const CONCURRENCY_TYPE(TExecutor) *executor,                            \
      CONCURRENCY_TYPE(TTask) task)
#define EXECUTOR_RUN_INLINE_PROTOTYPE                                          \
  static inline OPSTATUS CONCURRENCY_FUNC(Executor, RunInline)(               \
      CONCURRENCY_TYPE(TTask) task)

EXECUTOR_SUBMIT_PROTOTYPE;
EXECUTOR_RUN_INLINE_PROTOTYPE;

EXECUTOR_RUN_INLINE_PROTOTYPE {
  return TASK_FUNC(Run)(&task);
}
EXECUTOR_SUBMIT_PROTOTYPE {
  if (executor == NULL || executor->submit == NULL)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (task.function == NULL)
    return STATUS_CONST(INVALID_ARGUMENT);
  return executor->submit(executor->userData, task);
}
