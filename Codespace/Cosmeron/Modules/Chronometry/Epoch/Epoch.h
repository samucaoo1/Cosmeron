#pragma once

#include "../DateTime/DateTime.h"
#include "../Instant/Instant.h"

#define CHRONOMETRY_EPOCH_DATE_TO_JDN_PROTOTYPE                      \
  inline static OPSTATUS EPOCH_FUNC(DateToJulianDayNumber)(                     \
      const CALENDAR_FUNC(Policy) *policy, CHRONOMETRY_TYPE(Date) date,         \
      int64_t *julianDayNumber)
#define CHRONOMETRY_EPOCH_DATE_FROM_JDN_PROTOTYPE                    \
  inline static OPSTATUS EPOCH_FUNC(DateFromJulianDayNumber)(                   \
      const CALENDAR_FUNC(Policy) *policy, int64_t julianDayNumber,           \
      CHRONOMETRY_TYPE(Date) *date)
#define CHRONOMETRY_EPOCH_DATE_TO_RATA_DIE_PROTOTYPE                 \
  inline static OPSTATUS EPOCH_FUNC(DateToRataDie)(                             \
      CHRONOMETRY_TYPE(Date) date, int64_t *rataDie)
#define CHRONOMETRY_EPOCH_DATETIME_TO_UNIX_PROTOTYPE                 \
  inline static OPSTATUS EPOCH_FUNC(DateTimeToUnix)(                            \
      CHRONOMETRY_TYPE(DateTime) value, CHRONOMETRY_TYPE(Instant) *instant)
#define CHRONOMETRY_EPOCH_DATETIME_FROM_UNIX_PROTOTYPE               \
  inline static OPSTATUS EPOCH_FUNC(DateTimeFromUnix)(                          \
      CHRONOMETRY_TYPE(Instant) instant, CHRONOMETRY_TYPE(DateTime) *value)

#include "Impl/Epoch.impl"
