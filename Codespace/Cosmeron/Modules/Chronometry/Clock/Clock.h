#pragma once

#include "../Duration/Duration.h"

typedef struct CHRONOMETRY_TYPE(ClockTimePoint) {
  uint64_t nanoseconds;
} CHRONOMETRY_TYPE(ClockTimePoint);

#define CHRONOMETRY_CLOCK_NOW_PROTOTYPE                              \
  inline static OPSTATUS CLOCK_FUNC(Now)(                                       \
      CHRONOMETRY_TYPE(ClockTimePoint) *result)
#define CHRONOMETRY_CLOCK_DURATION_BETWEEN_PROTOTYPE                 \
  inline static OPSTATUS CLOCK_FUNC(DurationBetween)(                           \
      CHRONOMETRY_TYPE(ClockTimePoint) start,                                 \
      CHRONOMETRY_TYPE(ClockTimePoint) end,                                   \
      CHRONOMETRY_TYPE(Duration) *result)

#include "Impl/Clock.impl"
