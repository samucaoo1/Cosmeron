#pragma once

#include "../Impl/Common.inc"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* 1. Decoupled X-Macro Tables (Row-Tuple Pattern)                            */
/* ========================================================================== */

#define CHRONOMETRY_CALENDAR_MONTH_COL_NAME(NAME, NUMBER, DAYS, BEFORE) NAME
#define CHRONOMETRY_CALENDAR_MONTH_COL_NUMBER(NAME, NUMBER, DAYS, BEFORE) NUMBER
#define CHRONOMETRY_CALENDAR_MONTH_COL_DAYS(NAME, NUMBER, DAYS, BEFORE) DAYS
#define CHRONOMETRY_CALENDAR_MONTH_COL_BEFORE(NAME, NUMBER, DAYS, BEFORE) BEFORE
#define CHRONOMETRY_CALENDAR_MONTH_GET(FIELD, ROW)                             \
  PP_OP_CAT2(CHRONOMETRY_CALENDAR_MONTH_COL_, FIELD) ROW

#define CHRONOMETRY_CALENDAR_MONTH_TABLE(X)                                    \
  X((JANUARY, 1, 31, 0))                                                       \
  X((FEBRUARY, 2, 28, 31))                                                     \
  X((MARCH, 3, 31, 59))                                                        \
  X((APRIL, 4, 30, 90))                                                        \
  X((MAY, 5, 31, 120))                                                         \
  X((JUNE, 6, 30, 151))                                                        \
  X((JULY, 7, 31, 181))                                                        \
  X((AUGUST, 8, 31, 212))                                                      \
  X((SEPTEMBER, 9, 30, 243))                                                   \
  X((OCTOBER, 10, 31, 273))                                                    \
  X((NOVEMBER, 11, 30, 304))                                                   \
  X((DECEMBER, 12, 31, 334))

#define CHRONOMETRY_CALENDAR_REFORM_COL_NAME(                                  \
    NAME, BEFORE_YEAR, BEFORE_MONTH, BEFORE_DAY, AFTER_YEAR, AFTER_MONTH,      \
    AFTER_DAY, SUPPRESSED)                                                     \
  NAME
#define CHRONOMETRY_CALENDAR_REFORM_COL_BEFORE_YEAR(                           \
    NAME, BEFORE_YEAR, BEFORE_MONTH, BEFORE_DAY, AFTER_YEAR, AFTER_MONTH,      \
    AFTER_DAY, SUPPRESSED)                                                     \
  BEFORE_YEAR
#define CHRONOMETRY_CALENDAR_REFORM_COL_BEFORE_MONTH(                          \
    NAME, BEFORE_YEAR, BEFORE_MONTH, BEFORE_DAY, AFTER_YEAR, AFTER_MONTH,      \
    AFTER_DAY, SUPPRESSED)                                                     \
  BEFORE_MONTH
#define CHRONOMETRY_CALENDAR_REFORM_COL_BEFORE_DAY(                            \
    NAME, BEFORE_YEAR, BEFORE_MONTH, BEFORE_DAY, AFTER_YEAR, AFTER_MONTH,      \
    AFTER_DAY, SUPPRESSED)                                                     \
  BEFORE_DAY
#define CHRONOMETRY_CALENDAR_REFORM_COL_AFTER_YEAR(                            \
    NAME, BEFORE_YEAR, BEFORE_MONTH, BEFORE_DAY, AFTER_YEAR, AFTER_MONTH,      \
    AFTER_DAY, SUPPRESSED)                                                     \
  AFTER_YEAR
#define CHRONOMETRY_CALENDAR_REFORM_COL_AFTER_MONTH(                           \
    NAME, BEFORE_YEAR, BEFORE_MONTH, BEFORE_DAY, AFTER_YEAR, AFTER_MONTH,      \
    AFTER_DAY, SUPPRESSED)                                                     \
  AFTER_MONTH
#define CHRONOMETRY_CALENDAR_REFORM_COL_AFTER_DAY(                             \
    NAME, BEFORE_YEAR, BEFORE_MONTH, BEFORE_DAY, AFTER_YEAR, AFTER_MONTH,      \
    AFTER_DAY, SUPPRESSED)                                                     \
  AFTER_DAY
#define CHRONOMETRY_CALENDAR_REFORM_COL_SUPPRESSED(                            \
    NAME, BEFORE_YEAR, BEFORE_MONTH, BEFORE_DAY, AFTER_YEAR, AFTER_MONTH,      \
    AFTER_DAY, SUPPRESSED)                                                     \
  SUPPRESSED
#define CHRONOMETRY_CALENDAR_REFORM_GET(FIELD, ROW)                            \
  PP_OP_CAT2(CHRONOMETRY_CALENDAR_REFORM_COL_, FIELD) ROW

#define CHRONOMETRY_CALENDAR_REFORM_TABLE(X)                                   \
  X((CATHOLIC_1582, 1582, OCTOBER, 4, 1582, OCTOBER, 15, 10))                  \
  X((BRITISH_1752, 1752, SEPTEMBER, 2, 1752, SEPTEMBER, 14, 11))               \
  X((RUSSIAN_1918, 1918, JANUARY, 31, 1918, FEBRUARY, 14, 13))

#define CHRONOMETRY_CALENDAR_SYSTEM_COL_NAME(NAME) NAME
#define CHRONOMETRY_CALENDAR_SYSTEM_GET(FIELD, ROW)                            \
  PP_OP_CAT2(CHRONOMETRY_CALENDAR_SYSTEM_COL_, FIELD) ROW

#define CHRONOMETRY_CALENDAR_SYSTEM_TABLE(X)                                   \
  X((GREGORIAN))                                                               \
  X((JULIAN))

#define CHRONOMETRY_CALENDAR_POLICY_KIND_COL_NAME(NAME) NAME
#define CHRONOMETRY_CALENDAR_POLICY_KIND_GET(FIELD, ROW)                       \
  PP_OP_CAT2(CHRONOMETRY_CALENDAR_POLICY_KIND_COL_, FIELD) ROW

#define CHRONOMETRY_CALENDAR_POLICY_KIND_TABLE(X)                              \
  X((PURE_SYSTEM))                                                             \
  X((HISTORICAL_REFORM))

/* ========================================================================== */
/* 2. Core Calendar Enums (Typed Fields for Structs & Signatures)             */
/* ========================================================================== */

#define COSMERON_MACRO_INTERNAL_CHRONOMETRY_SYS_DISP(TARGET, ENUM_NAME, ROW)   \
  CNS(TARGET, CHRONOMETRY_CALENDAR_SYSTEM_GET(NAME, ROW)),

#define COSMERON_MACRO_INTERNAL_CHRONOMETRY_KIND_DISP(TARGET, ENUM_NAME, ROW)  \
  CNS(TARGET, CHRONOMETRY_CALENDAR_POLICY_KIND_GET(NAME, ROW)),

ENUM_DEF_EX(MODULE_NS(Chronometry, Calendar), System,
            CHRONOMETRY_CALENDAR_SYSTEM_TABLE,
            COSMERON_MACRO_INTERNAL_CHRONOMETRY_SYS_DISP)
ENUM_DEF_EX(MODULE_NS(Chronometry, Calendar), PolicyKind,
            CHRONOMETRY_CALENDAR_POLICY_KIND_TABLE,
            COSMERON_MACRO_INTERNAL_CHRONOMETRY_KIND_DISP)

#undef COSMERON_MACRO_INTERNAL_CHRONOMETRY_KIND_DISP
#undef COSMERON_MACRO_INTERNAL_CHRONOMETRY_SYS_DISP

/* ========================================================================== */
/* 3. Declarative Structs & Calendar Package                                  */
/* ========================================================================== */

#define COSMERON_MACRO_INTERNAL_CHRONOMETRY_MONTH_DISP(TARGET, ENUM_NAME, ROW) \
  CNS(TARGET, CHRONOMETRY_CALENDAR_MONTH_GET(NAME, ROW)) =                     \
      CHRONOMETRY_CALENDAR_MONTH_GET(NUMBER, ROW),

#define COSMERON_MACRO_INTERNAL_CHRONOMETRY_MONTH_LEN_DISP(TARGET, ENUM_NAME,  \
                                                           ROW)                \
  CNS(TARGET,                                                                  \
      PP_OP_CAT2(DAYS_IN_, CHRONOMETRY_CALENDAR_MONTH_GET(NAME, ROW))) =       \
      CHRONOMETRY_CALENDAR_MONTH_GET(DAYS, ROW),

#define COSMERON_MACRO_INTERNAL_CHRONOMETRY_REFORM_DISP(TARGET, ENUM_NAME,     \
                                                        ROW)                   \
  CNS(TARGET, CHRONOMETRY_CALENDAR_REFORM_GET(NAME, ROW)),

MODULE(
    DOMAIN(Modules), Chronometry,
    STRUCT(Date,
           FIELDS(int32_t year : 23; uint8_t month : 4; uint8_t day : 5;)),

    STRUCT(CalendarReform,
           FIELDS(MODULE_ENUM(Chronometry, Calendar, System) before;
                  MODULE_ENUM(Chronometry, Calendar, System) after;
                  MODULE_TYPE(Chronometry, Date) lastBefore;
                  MODULE_TYPE(Chronometry, Date) firstAfter;
                  uint8_t suppressedDays;)),

    STRUCT(CalendarPolicy,
           FIELDS(MODULE_ENUM(Chronometry, Calendar, PolicyKind) kind;
                  MODULE_ENUM(Chronometry, Calendar, System) system;
                  MODULE_TYPE(Chronometry, CalendarReform) reform;)),

    PACKAGE(
        Calendar,
        ENUM_EX(Month, CHRONOMETRY_CALENDAR_MONTH_TABLE,
                COSMERON_MACRO_INTERNAL_CHRONOMETRY_MONTH_DISP),
        ENUM_EX(MonthLength, CHRONOMETRY_CALENDAR_MONTH_TABLE,
                COSMERON_MACRO_INTERNAL_CHRONOMETRY_MONTH_LEN_DISP),
        ENUM_EX(ReformId, CHRONOMETRY_CALENDAR_REFORM_TABLE,
                COSMERON_MACRO_INTERNAL_CHRONOMETRY_REFORM_DISP),

        OPERATION(SystemPolicy,
                  (MODULE_ENUM(Chronometry, Calendar, System) system,
                   MODULE_TYPE(Chronometry, CalendarPolicy) * outPolicy)),
        OPERATION(ReformPolicy,
                  (MODULE_ENUM(Chronometry, Calendar, ReformId) id,
                   MODULE_TYPE(Chronometry, CalendarPolicy) * outPolicy)),
        PREDICATE(IsLeapYear,
                  (MODULE_ENUM(Chronometry, Calendar, System) system,
                   int32_t year)),
        OPERATION(DaysInMonth,
                  (MODULE_ENUM(Chronometry, Calendar, System) system,
                   int32_t year, uint8_t month, uint8_t *outDays)),
        VARIANT(DayOfYear,
                OPERATION((const MODULE_TYPE(Chronometry, CalendarPolicy) *
                               policy,
                           MODULE_TYPE(Chronometry, Date) date,
                           uint16_t *outDayOfYear)),
                OPERATION(Cumulative,
                          (MODULE_ENUM(Chronometry, Calendar, System) system,
                           MODULE_TYPE(Chronometry, Date) date,
                           uint16_t *outDayOfYear))),

        SPACE(
            Date,
            COMPARISON(Compare, (MODULE_TYPE(Chronometry, Date) left,
                                 MODULE_TYPE(Chronometry, Date) right)),
            PREDICATE(IsValid,
                      (const MODULE_TYPE(Chronometry, CalendarPolicy) * policy,
                       MODULE_TYPE(Chronometry, Date) date)),
            OPERATION(ToDayOfYear,
                      (const MODULE_TYPE(Chronometry, CalendarPolicy) * policy,
                       MODULE_TYPE(Chronometry, Date) date,
                       uint16_t *outDayOfYear)),
            OPERATION(ToOrdinal,
                      (const MODULE_TYPE(Chronometry, CalendarPolicy) * policy,
                       MODULE_TYPE(Chronometry, Date) date,
                       int64_t *outOrdinal)),
            OPERATION(FromOrdinal,
                      (const MODULE_TYPE(Chronometry, CalendarPolicy) * policy,
                       int64_t ordinal,
                       MODULE_TYPE(Chronometry, Date) * outDate)),
            OPERATION(Difference,
                      (const MODULE_TYPE(Chronometry, CalendarPolicy) * policy,
                       MODULE_TYPE(Chronometry, Date) left,
                       MODULE_TYPE(Chronometry, Date) right,
                       int64_t *outDays)))));

#undef COSMERON_MACRO_INTERNAL_CHRONOMETRY_REFORM_DISP
#undef COSMERON_MACRO_INTERNAL_CHRONOMETRY_MONTH_LEN_DISP
#undef COSMERON_MACRO_INTERNAL_CHRONOMETRY_MONTH_DISP

#include "Impl/Calendar.impl"

/* EOF */
