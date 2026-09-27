#pragma once

#include "../../../Core/Error/Status.h"
#include "../Chronometry.space"
#include <stddef.h>

#define CHRONOMETRY_CALENDAR_MONTH_TABLE(X)                                   \
  X(JANUARY, 1, 31)                                                           \
  X(FEBRUARY, 2, 28)                                                          \
  X(MARCH, 3, 31)                                                             \
  X(APRIL, 4, 30)                                                             \
  X(MAY, 5, 31)                                                               \
  X(JUNE, 6, 30)                                                              \
  X(JULY, 7, 31)                                                              \
  X(AUGUST, 8, 31)                                                            \
  X(SEPTEMBER, 9, 30)                                                         \
  X(OCTOBER, 10, 31)                                                          \
  X(NOVEMBER, 11, 30)                                                         \
  X(DECEMBER, 12, 31)

#define CHRONOMETRY_CALENDAR_REFORM_TABLE(X)                                  \
  X(CATHOLIC_1582, 1582, OCTOBER, 4, 1582, OCTOBER, 15, 10)                   \
  X(BRITISH_1752, 1752, SEPTEMBER, 2, 1752, SEPTEMBER, 14, 11)                \
  X(RUSSIAN_1918, 1918, JANUARY, 31, 1918, FEBRUARY, 14, 13)

typedef enum CALENDAR_FUNC(Month) {
#define X(NAME, NUMBER, DAYS) CALENDAR_CONST(NAME) = NUMBER,
  CHRONOMETRY_CALENDAR_MONTH_TABLE(X)
#undef X
} CALENDAR_FUNC(Month);

typedef enum CALENDAR_FUNC(MonthLength) {
#define X(NAME, NUMBER, DAYS) CALENDAR_FUNC(DaysIn##NAME) = DAYS,
  CHRONOMETRY_CALENDAR_MONTH_TABLE(X)
#undef X
} CALENDAR_FUNC(MonthLength);

typedef enum CALENDAR_FUNC(System) {
  CALENDAR_CONST(GREGORIAN),
  CALENDAR_CONST(JULIAN)
} CALENDAR_FUNC(System);

typedef struct CHRONOMETRY_TYPE(Date) {
  int32_t year :23;
  uint8_t month : 4;
  uint8_t day : 5;
} CHRONOMETRY_TYPE(Date);

typedef struct CALENDAR_FUNC(Reform) {
  CALENDAR_FUNC(System) before;
  CALENDAR_FUNC(System) after;
  CHRONOMETRY_TYPE(Date) lastBefore;
  CHRONOMETRY_TYPE(Date) firstAfter;
  uint8_t suppressedDays;
} CALENDAR_FUNC(Reform);

typedef enum CALENDAR_FUNC(ReformId) {
#define X(NAME, ...) CALENDAR_CONST(NAME),
  CHRONOMETRY_CALENDAR_REFORM_TABLE(X)
#undef X
} CALENDAR_FUNC(ReformId);

typedef enum CALENDAR_FUNC(PolicyKind) {
  CALENDAR_CONST(PURE_SYSTEM),
  CALENDAR_CONST(HISTORICAL_REFORM)
} CALENDAR_FUNC(PolicyKind);

typedef struct CALENDAR_FUNC(Policy) {
  CALENDAR_FUNC(PolicyKind) kind;
  CALENDAR_FUNC(System) system;
  CALENDAR_FUNC(Reform) reform;
} CALENDAR_FUNC(Policy);

#define CHRONOMETRY_CALENDAR_SYSTEM_POLICY_PROTOTYPE                 \
  inline static CALENDAR_FUNC(Policy) CALENDAR_FUNC(SystemPolicy)(                \
      CALENDAR_FUNC(System) system)
#define CHRONOMETRY_CALENDAR_REFORM_POLICY_PROTOTYPE                 \
  inline static OPSTATUS CALENDAR_FUNC(ReformPolicy)(                           \
      CALENDAR_FUNC(ReformId) id, CALENDAR_FUNC(Policy) *policy)
#define CHRONOMETRY_CALENDAR_DATE_COMPARE_PROTOTYPE                  \
  inline static int CALENDAR_FUNC(DateCompare)(CHRONOMETRY_TYPE(Date) left,     \
                                              CHRONOMETRY_TYPE(Date) right)
#define CHRONOMETRY_CALENDAR_IS_LEAP_YEAR_PROTOTYPE                  \
  inline static bool CALENDAR_FUNC(IsLeapYear)(CALENDAR_FUNC(System) system,      \
                                              int32_t year)
#define CHRONOMETRY_CALENDAR_DAYS_IN_MONTH_PROTOTYPE                 \
  inline static uint8_t CALENDAR_FUNC(DaysInMonth)(CALENDAR_FUNC(System) system,  \
                                                  int32_t year, uint8_t month)
#define CHRONOMETRY_CALENDAR_DATE_IS_VALID_PROTOTYPE                 \
  inline static bool CALENDAR_FUNC(DateIsValid)(                                \
      const CALENDAR_FUNC(Policy) *policy, CHRONOMETRY_TYPE(Date) date)
#define CHRONOMETRY_CALENDAR_DATE_TO_DAY_OF_YEAR_PROTOTYPE           \
  inline static OPSTATUS CALENDAR_FUNC(DateToDayOfYear)(                        \
      const CALENDAR_FUNC(Policy) *policy, CHRONOMETRY_TYPE(Date) date,         \
      uint16_t *dayOfYear)
#define CHRONOMETRY_CALENDAR_DATE_TO_ORDINAL_PROTOTYPE               \
  inline static OPSTATUS CALENDAR_FUNC(DateToOrdinal)(                          \
      const CALENDAR_FUNC(Policy) *policy, CHRONOMETRY_TYPE(Date) date,         \
      int64_t *ordinal)
#define CHRONOMETRY_CALENDAR_DATE_FROM_ORDINAL_PROTOTYPE             \
  inline static OPSTATUS CALENDAR_FUNC(DateFromOrdinal)(                        \
      const CALENDAR_FUNC(Policy) *policy, int64_t ordinal,                     \
      CHRONOMETRY_TYPE(Date) *date)
#define CHRONOMETRY_CALENDAR_DATE_DIFFERENCE_PROTOTYPE               \
  inline static OPSTATUS CALENDAR_FUNC(DateDifference)(                         \
      const CALENDAR_FUNC(Policy) *policy, CHRONOMETRY_TYPE(Date) left,         \
      CHRONOMETRY_TYPE(Date) right, int64_t *days)

#include "Impl/Calendar.impl"
