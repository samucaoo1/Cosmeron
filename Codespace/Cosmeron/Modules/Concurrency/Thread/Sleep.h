#pragma once

#include "../../Chronometry/Clock/Clock.h"
#include "Thread.h"

#define THREAD_SLEEP_FOR_PROTOTYPE                                   \
  static inline OPSTATUS THREAD_FUNC(SleepFor)(                                 \
      CHRONOMETRY_TYPE(Duration) duration)
#define THREAD_SLEEP_UNTIL_PROTOTYPE                                 \
  static inline OPSTATUS THREAD_FUNC(SleepUntil)(                               \
      CHRONOMETRY_TYPE(ClockTimePoint) deadline)

#include "Impl/Sleep.impl"

/* EOF */
