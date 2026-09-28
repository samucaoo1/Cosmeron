#pragma once

#include "../../../Core/Error/Status.h"
#include "../Chronometry.space"
#include <stddef.h>

#define CHRONOMETRY_NANOSECONDS_PER_MICROSECOND INT64_C(1000)
#define CHRONOMETRY_NANOSECONDS_PER_MILLISECOND INT64_C(1000000)
#define CHRONOMETRY_NANOSECONDS_PER_SECOND INT64_C(1000000000)
#define CHRONOMETRY_MILLISECONDS_PER_SECOND INT64_C(1000)
#define CHRONOMETRY_SECONDS_PER_MINUTE INT64_C(60)
#define CHRONOMETRY_MINUTES_PER_HOUR INT64_C(60)
#define CHRONOMETRY_HOURS_PER_DAY INT64_C(24)

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
