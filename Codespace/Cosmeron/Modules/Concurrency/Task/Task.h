#pragma once
#include "../../../Core/Error/Status.h"
#include "../Concurrency.space"

typedef void (*CONCURRENCY_TYPE(TaskFunction))(void *);

typedef struct CONCURRENCY_TYPE(TTask) {
  CONCURRENCY_TYPE(TaskFunction) function;
  void *argument;
} CONCURRENCY_TYPE(TTask);

static inline CONCURRENCY_TYPE(TTask) TASK_FUNC(Create)(CONCURRENCY_TYPE(TaskFunction) function, void *argument);
static inline bool TASK_FUNC(IsValid)(const CONCURRENCY_TYPE(TTask) *task);
static inline OPSTATUS TASK_FUNC(Run)(const CONCURRENCY_TYPE(TTask) *task);

#include "Impl/Task.impl"
