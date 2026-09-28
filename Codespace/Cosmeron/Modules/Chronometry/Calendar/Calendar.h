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

#define CALENDAR_MONTH_LENGTH_CONST(NAME)                                      \
  CALENDAR_CONST(PP_OP_CAT2(DAYS_IN_, NAME))

typedef enum CALENDAR_TYPE(Month) {
#define X(NAME, NUMBER, DAYS) CALENDAR_CONST(NAME) = NUMBER,
  CHRONOMETRY_CALENDAR_MONTH_TABLE(X)
#undef X
} CALENDAR_TYPE(Month);

typedef enum CALENDAR_TYPE(MonthLength) {
#define X(NAME, NUMBER, DAYS) CALENDAR_MONTH_LENGTH_CONST(NAME) = DAYS,
  CHRONOMETRY_CALENDAR_MONTH_TABLE(X)
#undef X
} CALENDAR_TYPE(MonthLength);

typedef enum CALENDAR_TYPE(System) {
  CALENDAR_CONST(GREGORIAN),
  CALENDAR_CONST(JULIAN)
} CALENDAR_TYPE(System);

typedef struct CHRONOMETRY_TYPE(Date) {
  int32_t year : 23;
  uint8_t month : 4;
  uint8_t day : 5;
} CHRONOMETRY_TYPE(Date);

typedef struct CALENDAR_TYPE(Reform) {
  CALENDAR_TYPE(System) before;
  CALENDAR_TYPE(System) after;
  CHRONOMETRY_TYPE(Date) lastBefore;
  CHRONOMETRY_TYPE(Date) firstAfter;
  uint8_t suppressedDays;
} CALENDAR_TYPE(Reform);

typedef enum CALENDAR_TYPE(ReformId) {
#define X(NAME, ...) CALENDAR_CONST(NAME),
  CHRONOMETRY_CALENDAR_REFORM_TABLE(X)
#undef X
} CALENDAR_TYPE(ReformId);

typedef enum CALENDAR_TYPE(PolicyKind) {
  CALENDAR_CONST(PURE_SYSTEM),
  CALENDAR_CONST(HISTORICAL_REFORM)
} CALENDAR_TYPE(PolicyKind);

typedef struct CALENDAR_TYPE(Policy) {
  CALENDAR_TYPE(PolicyKind) kind;
  CALENDAR_TYPE(System) system;
  CALENDAR_TYPE(Reform) reform;
} CALENDAR_TYPE(Policy);

#define CHRONOMETRY_CALENDAR_SYSTEM_POLICY_PROTOTYPE                           \
  static inline CALENDAR_TYPE(Policy) CALENDAR_FUNC(SystemPolicy)(             \
      CALENDAR_TYPE(System) system)

#define CHRONOMETRY_CALENDAR_REFORM_POLICY_PROTOTYPE                           \
  static inline OPSTATUS CALENDAR_FUNC(ReformPolicy)(                          \
      CALENDAR_TYPE(ReformId) id, CALENDAR_TYPE(Policy) *outPolicy)

#define CHRONOMETRY_CALENDAR_DATE_COMPARE_PROTOTYPE                            \
  static inline CMPOUT CALENDAR_FUNC(DateCompare)(                             \
      CHRONOMETRY_TYPE(Date) left, CHRONOMETRY_TYPE(Date) right)

#define CHRONOMETRY_CALENDAR_IS_LEAP_YEAR_PROTOTYPE                            \
  static inline bool CALENDAR_FUNC(IsLeapYear)(                                \
      CALENDAR_TYPE(System) system, int32_t year)

#define CHRONOMETRY_CALENDAR_DAYS_IN_MONTH_PROTOTYPE                           \
  static inline uint8_t CALENDAR_FUNC(DaysInMonth)(                            \
      CALENDAR_TYPE(System) system, int32_t year, uint8_t month)

#define CHRONOMETRY_CALENDAR_DATE_IS_VALID_PROTOTYPE                           \
  static inline bool CALENDAR_FUNC(DateIsValid)(                               \
      const CALENDAR_TYPE(Policy) *policy, CHRONOMETRY_TYPE(Date) date)

#define CHRONOMETRY_CALENDAR_DATE_TO_DAY_OF_YEAR_PROTOTYPE                     \
  static inline OPSTATUS CALENDAR_FUNC(DateToDayOfYear)(                       \
      const CALENDAR_TYPE(Policy) *policy, CHRONOMETRY_TYPE(Date) date,        \
      uint16_t *outDayOfYear)

#define CHRONOMETRY_CALENDAR_DATE_TO_ORDINAL_PROTOTYPE                         \
  static inline OPSTATUS CALENDAR_FUNC(DateToOrdinal)(                         \
      const CALENDAR_TYPE(Policy) *policy, CHRONOMETRY_TYPE(Date) date,        \
      int64_t *outOrdinal)

#define CHRONOMETRY_CALENDAR_DATE_FROM_ORDINAL_PROTOTYPE                       \
  static inline OPSTATUS CALENDAR_FUNC(DateFromOrdinal)(                       \
      const CALENDAR_TYPE(Policy) *policy, int64_t ordinal,                    \
      CHRONOMETRY_TYPE(Date) *outDate)

#define CHRONOMETRY_CALENDAR_DATE_DIFFERENCE_PROTOTYPE                         \
  static inline OPSTATUS CALENDAR_FUNC(DateDifference)(                        \
      const CALENDAR_TYPE(Policy) *policy, CHRONOMETRY_TYPE(Date) left,        \
      CHRONOMETRY_TYPE(Date) right, int64_t *outDays)

#include "Impl/Calendar.impl"
/* EOF */
