#pragma once

#include "../../../Core/Error/Status.h"
#include "../Chronometry.space"
#include <stddef.h>

enum {
  DURATION_CONST(NANOSECONDS_PER_MICROSECOND) = 1000,
  DURATION_CONST(NANOSECONDS_PER_MILLISECOND) = 1000000,
  DURATION_CONST(NANOSECONDS_PER_SECOND) = 1000000000,
  DURATION_CONST(MILLISECONDS_PER_SECOND) = 1000,
  DURATION_CONST(SECONDS_PER_MINUTE) = 60,
  DURATION_CONST(MINUTES_PER_HOUR) = 60,
  DURATION_CONST(HOURS_PER_DAY) = 24,
  DURATION_CONST(SECONDS_PER_HOUR) =
      DURATION_CONST(SECONDS_PER_MINUTE) * DURATION_CONST(MINUTES_PER_HOUR),
  DURATION_CONST(SECONDS_PER_DAY) =
      DURATION_CONST(SECONDS_PER_HOUR) * DURATION_CONST(HOURS_PER_DAY)
};

typedef struct CHRONOMETRY_TYPE(Duration) {
  int64_t nanoseconds;
} CHRONOMETRY_TYPE(Duration);

#define CHRONOMETRY_DURATION_FROM_NANOSECONDS_PROTOTYPE               \
  static inline CHRONOMETRY_TYPE(Duration) DURATION_FUNC(FromNanoseconds)(       \
      int64_t nanoseconds)
#define CHRONOMETRY_DURATION_FROM_SECONDS_PROTOTYPE                   \
  static inline OPSTATUS DURATION_FUNC(FromSeconds)(                             \
      int64_t seconds, CHRONOMETRY_TYPE(Duration) *outResult)
#define CHRONOMETRY_DURATION_ADD_PROTOTYPE                            \
  static inline OPSTATUS DURATION_FUNC(Add)(CHRONOMETRY_TYPE(Duration) left,     \
                                          CHRONOMETRY_TYPE(Duration) right,    \
                                          CHRONOMETRY_TYPE(Duration) *outResult)
#define CHRONOMETRY_DURATION_SUBTRACT_PROTOTYPE                       \
  static inline OPSTATUS DURATION_FUNC(Subtract)(                                \
      CHRONOMETRY_TYPE(Duration) left, CHRONOMETRY_TYPE(Duration) right,       \
      CHRONOMETRY_TYPE(Duration) *outResult)
#define CHRONOMETRY_DURATION_COMPARE_PROTOTYPE                        \
  static inline CMPOUT DURATION_FUNC(Compare)(CHRONOMETRY_TYPE(Duration) left,      \
                                         CHRONOMETRY_TYPE(Duration) right)
#define CHRONOMETRY_DURATION_ABSOLUTE_PROTOTYPE                       \
  static inline OPSTATUS DURATION_FUNC(Absolute)(                                \
      CHRONOMETRY_TYPE(Duration) value, CHRONOMETRY_TYPE(Duration) *outResult)

#include "Impl/Duration.impl"
