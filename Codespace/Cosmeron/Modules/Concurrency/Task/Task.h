#pragma once
#include "../../../Core/Error/Status.h"
#include "../Concurrency.space"

typedef void (*TTaskFunction)(void *);

typedef struct TTask {
  TTaskFunction function;
  void *argument;
} TTask;

inline static TTask TASK_FUNC(Create)(TTaskFunction function, void *argument);
inline static bool TASK_FUNC(IsValid)(const TTask *task);
inline static OPSTATUS TASK_FUNC(Run)(const TTask *task);

#include "Impl/Task.impl"
