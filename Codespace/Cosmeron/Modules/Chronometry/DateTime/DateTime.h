#pragma once

#include "../Calendar/Calendar.h"
#include "../Duration/Duration.h"

typedef struct CHRONOMETRY_TYPE(TimeOfDay) {
  uint16_t millisecond : 10;
  uint8_t second : 6;
  uint8_t minute : 6;
  uint8_t hour : 5;
} CHRONOMETRY_TYPE(TimeOfDay);

typedef struct CHRONOMETRY_TYPE(DateTime) {
  CHRONOMETRY_TYPE(Date) date;
  CHRONOMETRY_TYPE(TimeOfDay) time;
} CHRONOMETRY_TYPE(DateTime);

#define CHRONOMETRY_DATETIME_TIME_IS_VALID_PROTOTYPE                 \
  inline static bool DATETIME_FUNC(TimeIsValid)(CHRONOMETRY_TYPE(TimeOfDay) time)
#define CHRONOMETRY_DATETIME_IS_VALID_PROTOTYPE                      \
  inline static bool DATETIME_FUNC(IsValid)(                                    \
      const CALENDAR_FUNC(Policy) *policy, CHRONOMETRY_TYPE(DateTime) value)
#define CHRONOMETRY_DATETIME_COMPARE_PROTOTYPE                       \
  inline static int DATETIME_FUNC(Compare)(CHRONOMETRY_TYPE(DateTime) left,     \
                                         CHRONOMETRY_TYPE(DateTime) right)
#define CHRONOMETRY_DATETIME_ADD_DURATION_PROTOTYPE                  \
  inline static OPSTATUS DATETIME_FUNC(AddDuration)(                            \
      const CALENDAR_FUNC(Policy) *policy, CHRONOMETRY_TYPE(DateTime) value,    \
      CHRONOMETRY_TYPE(Duration) duration,                                    \
      CHRONOMETRY_TYPE(DateTime) *result)
#define CHRONOMETRY_DATETIME_DIFFERENCE_PROTOTYPE                    \
  inline static OPSTATUS DATETIME_FUNC(Difference)(                             \
      const CALENDAR_FUNC(Policy) *policy, CHRONOMETRY_TYPE(DateTime) left,     \
      CHRONOMETRY_TYPE(DateTime) right, CHRONOMETRY_TYPE(Duration) *result)

#include "Impl/DateTime.impl"
