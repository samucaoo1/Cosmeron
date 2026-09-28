#pragma once
#include "../../../Core/Error/Status.h"
#include "../Concurrency.space"

typedef void (*CONCURRENCY_TYPE(TaskFunction))(void *);

typedef struct CONCURRENCY_TYPE(TTask) {
  CONCURRENCY_TYPE(TaskFunction) function;
  void *argument;
} CONCURRENCY_TYPE(TTask);

#define TASK_CREATE_PROTOTYPE                                                 \
  static inline CONCURRENCY_TYPE(TTask) TASK_FUNC(Create)(                    \
      CONCURRENCY_TYPE(TaskFunction) function, void *argument)
#define TASK_IS_VALID_PROTOTYPE                                               \
  static inline bool TASK_FUNC(IsValid)(const CONCURRENCY_TYPE(TTask) *task)
#define TASK_RUN_PROTOTYPE                                                    \
  static inline OPSTATUS TASK_FUNC(Run)(const CONCURRENCY_TYPE(TTask) *task)

TASK_CREATE_PROTOTYPE;
TASK_IS_VALID_PROTOTYPE;
TASK_RUN_PROTOTYPE;

#include "Impl/Task.impl"
