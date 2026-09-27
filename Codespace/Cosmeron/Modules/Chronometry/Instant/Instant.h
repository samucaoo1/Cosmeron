#pragma once

#include "../Duration/Duration.h"

typedef struct CHRONOMETRY_TYPE(Instant) {
  int64_t seconds;
  uint32_t nanoseconds;
} CHRONOMETRY_TYPE(Instant);

#define CHRONOMETRY_INSTANT_COMPARE_PROTOTYPE                        \
  inline static int INSTANT_FUNC(Compare)(CHRONOMETRY_TYPE(Instant) left,       \
                                        CHRONOMETRY_TYPE(Instant) right)
#define CHRONOMETRY_INSTANT_ADD_DURATION_PROTOTYPE                   \
  inline static OPSTATUS INSTANT_FUNC(AddDuration)(                             \
      CHRONOMETRY_TYPE(Instant) instant, CHRONOMETRY_TYPE(Duration) duration, \
      CHRONOMETRY_TYPE(Instant) *result)
#define CHRONOMETRY_INSTANT_DURATION_BETWEEN_PROTOTYPE               \
  inline static OPSTATUS INSTANT_FUNC(DurationBetween)(                         \
      CHRONOMETRY_TYPE(Instant) start, CHRONOMETRY_TYPE(Instant) end,         \
      CHRONOMETRY_TYPE(Duration) *result)

#include "Impl/Instant.impl"
