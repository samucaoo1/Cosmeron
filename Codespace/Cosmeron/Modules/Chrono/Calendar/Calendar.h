#pragma once

#include "../Impl/Common.inc"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* 1. Decoupled X-Macro Tables (Row-Tuple Pattern)                            */
/* ========================================================================== */

#define CHRONO_CALENDAR_MONTH_COL_NAME(NAME, NUMBER, DAYS, BEFORE) NAME
#define CHRONO_CALENDAR_MONTH_COL_NUMBER(NAME, NUMBER, DAYS, BEFORE) NUMBER
#define CHRONO_CALENDAR_MONTH_COL_DAYS(NAME, NUMBER, DAYS, BEFORE) DAYS
#define CHRONO_CALENDAR_MONTH_COL_BEFORE(NAME, NUMBER, DAYS, BEFORE) BEFORE
#define CHRONO_CALENDAR_MONTH_GET(FIELD, ROW)                                  \
  PP_OP_CAT2(CHRONO_CALENDAR_MONTH_COL_, FIELD) ROW

#define CHRONO_CALENDAR_MONTH_TABLE(X)                                         \
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

#define CHRONO_CALENDAR_REFORM_COL_NAME(                                       \
    NAME, BEFORE_YEAR, BEFORE_MONTH, BEFORE_DAY, AFTER_YEAR, AFTER_MONTH,      \
    AFTER_DAY, SUPPRESSED)                                                     \
  NAME
#define CHRONO_CALENDAR_REFORM_COL_BEFORE_YEAR(                                \
    NAME, BEFORE_YEAR, BEFORE_MONTH, BEFORE_DAY, AFTER_YEAR, AFTER_MONTH,      \
    AFTER_DAY, SUPPRESSED)                                                     \
  BEFORE_YEAR
#define CHRONO_CALENDAR_REFORM_COL_BEFORE_MONTH(                               \
    NAME, BEFORE_YEAR, BEFORE_MONTH, BEFORE_DAY, AFTER_YEAR, AFTER_MONTH,      \
    AFTER_DAY, SUPPRESSED)                                                     \
  BEFORE_MONTH
#define CHRONO_CALENDAR_REFORM_COL_BEFORE_DAY(                                 \
    NAME, BEFORE_YEAR, BEFORE_MONTH, BEFORE_DAY, AFTER_YEAR, AFTER_MONTH,      \
    AFTER_DAY, SUPPRESSED)                                                     \
  BEFORE_DAY
#define CHRONO_CALENDAR_REFORM_COL_AFTER_YEAR(                                 \
    NAME, BEFORE_YEAR, BEFORE_MONTH, BEFORE_DAY, AFTER_YEAR, AFTER_MONTH,      \
    AFTER_DAY, SUPPRESSED)                                                     \
  AFTER_YEAR
#define CHRONO_CALENDAR_REFORM_COL_AFTER_MONTH(                                \
    NAME, BEFORE_YEAR, BEFORE_MONTH, BEFORE_DAY, AFTER_YEAR, AFTER_MONTH,      \
    AFTER_DAY, SUPPRESSED)                                                     \
  AFTER_MONTH
#define CHRONO_CALENDAR_REFORM_COL_AFTER_DAY(                                  \
    NAME, BEFORE_YEAR, BEFORE_MONTH, BEFORE_DAY, AFTER_YEAR, AFTER_MONTH,      \
    AFTER_DAY, SUPPRESSED)                                                     \
  AFTER_DAY
#define CHRONO_CALENDAR_REFORM_COL_SUPPRESSED(                                 \
    NAME, BEFORE_YEAR, BEFORE_MONTH, BEFORE_DAY, AFTER_YEAR, AFTER_MONTH,      \
    AFTER_DAY, SUPPRESSED)                                                     \
  SUPPRESSED
#define CHRONO_CALENDAR_REFORM_GET(FIELD, ROW)                                 \
  PP_OP_CAT2(CHRONO_CALENDAR_REFORM_COL_, FIELD) ROW

#define CHRONO_CALENDAR_REFORM_TABLE(X)                                        \
  X((CATHOLIC_1582, 1582, OCTOBER, 4, 1582, OCTOBER, 15, 10))                  \
  X((BRITISH_1752, 1752, SEPTEMBER, 2, 1752, SEPTEMBER, 14, 11))               \
  X((RUSSIAN_1918, 1918, JANUARY, 31, 1918, FEBRUARY, 14, 13))

#define CHRONO_CALENDAR_SYSTEM_COL_NAME(NAME) NAME
#define CHRONO_CALENDAR_SYSTEM_GET(FIELD, ROW)                                 \
  PP_OP_CAT2(CHRONO_CALENDAR_SYSTEM_COL_, FIELD) ROW

#define CHRONO_CALENDAR_SYSTEM_TABLE(X)                                        \
  X((GREGORIAN))                                                               \
  X((JULIAN))

#define CHRONO_CALENDAR_POLICY_KIND_COL_NAME(NAME) NAME
#define CHRONO_CALENDAR_POLICY_KIND_GET(FIELD, ROW)                            \
  PP_OP_CAT2(CHRONO_CALENDAR_POLICY_KIND_COL_, FIELD) ROW

#define CHRONO_CALENDAR_POLICY_KIND_TABLE(X)                                   \
  X((PURE_SYSTEM))                                                             \
  X((HISTORICAL_REFORM))

#define CHRONO_CALENDAR_WEEKDAY_COL_NAME(NAME, ISO_NUMBER) NAME
#define CHRONO_CALENDAR_WEEKDAY_COL_ISO_NUMBER(NAME, ISO_NUMBER) ISO_NUMBER
#define CHRONO_CALENDAR_WEEKDAY_GET(FIELD, ROW)                                \
  PP_OP_CAT2(CHRONO_CALENDAR_WEEKDAY_COL_, FIELD) ROW

#define CHRONO_CALENDAR_WEEKDAY_TABLE(X)                                       \
  X((MONDAY, 1))                                                               \
  X((TUESDAY, 2))                                                              \
  X((WEDNESDAY, 3))                                                            \
  X((THURSDAY, 4))                                                             \
  X((FRIDAY, 5))                                                               \
  X((SATURDAY, 6))                                                             \
  X((SUNDAY, 7))

#define CHRONO_CALENDAR_WEEK_OCCURRENCE_COL_NAME(NAME, INDEX) NAME
#define CHRONO_CALENDAR_WEEK_OCCURRENCE_COL_INDEX(NAME, INDEX) INDEX
#define CHRONO_CALENDAR_WEEK_OCCURRENCE_GET(FIELD, ROW)                        \
  PP_OP_CAT2(CHRONO_CALENDAR_WEEK_OCCURRENCE_COL_, FIELD) ROW

#define CHRONO_CALENDAR_WEEK_OCCURRENCE_TABLE(X)                               \
  X((FIRST_WEEKDAY, 1))                                                        \
  X((SECOND_WEEKDAY, 2))                                                       \
  X((THIRD_WEEKDAY, 3))                                                        \
  X((FOURTH_WEEKDAY, 4))                                                       \
  X((LAST_WEEKDAY, 5))

#define CHRONO_CALENDAR_WEEKDAY_ANCHOR_COL_NAME(NAME) NAME
#define CHRONO_CALENDAR_WEEKDAY_ANCHOR_GET(FIELD, ROW)                         \
  PP_OP_CAT2(CHRONO_CALENDAR_WEEKDAY_ANCHOR_COL_, FIELD) ROW

#define CHRONO_CALENDAR_WEEKDAY_ANCHOR_TABLE(X)                                \
  X((WEEKDAY_EXACT))                                                           \
  X((WEEKDAY_ON_OR_AFTER))                                                     \
  X((WEEKDAY_ON_OR_BEFORE))                                                    \
  X((WEEKDAY_LAST))

/* ========================================================================== */
/* 2. Core Calendar Enums (Typed Fields for Structs & Signatures)             */
/* ========================================================================== */

#define COSMERON_MACRO_INTERNAL_CHRONO_SYS_DISP(TARGET, ENUM_NAME, ROW)        \
  CNS(TARGET, CHRONO_CALENDAR_SYSTEM_GET(NAME, ROW)),

#define COSMERON_MACRO_INTERNAL_CHRONO_KIND_DISP(TARGET, ENUM_NAME, ROW)       \
  CNS(TARGET, CHRONO_CALENDAR_POLICY_KIND_GET(NAME, ROW)),

#define COSMERON_MACRO_INTERNAL_CHRONO_WEEKDAY_DISP(TARGET, ENUM_NAME, ROW)    \
  CNS(TARGET, CHRONO_CALENDAR_WEEKDAY_GET(NAME, ROW)) =                        \
      CHRONO_CALENDAR_WEEKDAY_GET(ISO_NUMBER, ROW),

#define COSMERON_MACRO_INTERNAL_CHRONO_OCCURRENCE_DISP(TARGET, ENUM_NAME, ROW) \
  CNS(TARGET, CHRONO_CALENDAR_WEEK_OCCURRENCE_GET(NAME, ROW)) =                \
      CHRONO_CALENDAR_WEEK_OCCURRENCE_GET(INDEX, ROW),

#define COSMERON_MACRO_INTERNAL_CHRONO_ANCHOR_DISP(TARGET, ENUM_NAME, ROW)     \
  CNS(TARGET, CHRONO_CALENDAR_WEEKDAY_ANCHOR_GET(NAME, ROW)),

ENUM_DEF_EX(MODULE_NS(Chrono, Calendar), System,
            CHRONO_CALENDAR_SYSTEM_TABLE,
            COSMERON_MACRO_INTERNAL_CHRONO_SYS_DISP)
ENUM_DEF_EX(MODULE_NS(Chrono, Calendar), PolicyKind,
            CHRONO_CALENDAR_POLICY_KIND_TABLE,
            COSMERON_MACRO_INTERNAL_CHRONO_KIND_DISP)
ENUM_DEF_EX(MODULE_NS(Chrono, Calendar), Weekday,
            CHRONO_CALENDAR_WEEKDAY_TABLE,
            COSMERON_MACRO_INTERNAL_CHRONO_WEEKDAY_DISP)
ENUM_DEF_EX(MODULE_NS(Chrono, Calendar), WeekdayOccurrence,
            CHRONO_CALENDAR_WEEK_OCCURRENCE_TABLE,
            COSMERON_MACRO_INTERNAL_CHRONO_OCCURRENCE_DISP)
ENUM_DEF_EX(MODULE_NS(Chrono, Calendar), WeekdayAnchor,
            CHRONO_CALENDAR_WEEKDAY_ANCHOR_TABLE,
            COSMERON_MACRO_INTERNAL_CHRONO_ANCHOR_DISP)

#undef COSMERON_MACRO_INTERNAL_CHRONO_ANCHOR_DISP
#undef COSMERON_MACRO_INTERNAL_CHRONO_OCCURRENCE_DISP
#undef COSMERON_MACRO_INTERNAL_CHRONO_WEEKDAY_DISP
#undef COSMERON_MACRO_INTERNAL_CHRONO_KIND_DISP
#undef COSMERON_MACRO_INTERNAL_CHRONO_SYS_DISP

/* ========================================================================== */
/* 3. Declarative Structs & Calendar Package                                  */
/* ========================================================================== */

#define COSMERON_MACRO_INTERNAL_CHRONO_MONTH_DISP(TARGET, ENUM_NAME, ROW)      \
  CNS(TARGET, CHRONO_CALENDAR_MONTH_GET(NAME, ROW)) =                          \
      CHRONO_CALENDAR_MONTH_GET(NUMBER, ROW),

#define COSMERON_MACRO_INTERNAL_CHRONO_MONTH_LEN_DISP(TARGET, ENUM_NAME, ROW)  \
  CNS(TARGET,                                                                  \
      PP_OP_CAT2(DAYS_IN_, CHRONO_CALENDAR_MONTH_GET(NAME, ROW))) =            \
      CHRONO_CALENDAR_MONTH_GET(DAYS, ROW),

#define COSMERON_MACRO_INTERNAL_CHRONO_REFORM_DISP(TARGET, ENUM_NAME, ROW)     \
  CNS(TARGET, CHRONO_CALENDAR_REFORM_GET(NAME, ROW)),

MODULE(
    DOMAIN(Modules), Chrono,
    STRUCT(Date,
           FIELDS(int32_t year : 23; uint8_t month : 4; uint8_t day : 5;)),

    STRUCT(DateIsoWeek,
           FIELDS(int32_t year; uint8_t week : 6; uint8_t weekday : 4;)),

    STRUCT(CalendarReform,
           FIELDS(MODULE_ENUM(Chrono, Calendar, System) before;
                  MODULE_ENUM(Chrono, Calendar, System) after;
                  MODULE_TYPE(Chrono, Date) lastBefore;
                  MODULE_TYPE(Chrono, Date) firstAfter;
                  uint8_t suppressedDays;)),

    STRUCT(CalendarPolicy,
           FIELDS(MODULE_ENUM(Chrono, Calendar, PolicyKind) kind;
                  MODULE_ENUM(Chrono, Calendar, System) system;
                  MODULE_TYPE(Chrono, CalendarReform) reform;)),

    PACKAGE(
        Calendar,
        ENUM_EX(Month, CHRONO_CALENDAR_MONTH_TABLE,
                COSMERON_MACRO_INTERNAL_CHRONO_MONTH_DISP),
        ENUM_EX(MonthLength, CHRONO_CALENDAR_MONTH_TABLE,
                COSMERON_MACRO_INTERNAL_CHRONO_MONTH_LEN_DISP),
        ENUM_EX(ReformId, CHRONO_CALENDAR_REFORM_TABLE,
                COSMERON_MACRO_INTERNAL_CHRONO_REFORM_DISP),

        FUNCTION(MODULE_TYPE(Chrono, CalendarPolicy), GregorianPolicy, ()),
        OPERATION(SystemPolicy,
                  (MODULE_ENUM(Chrono, Calendar, System) system,
                   MODULE_TYPE(Chrono, CalendarPolicy) *outPolicy)),
        OPERATION(ReformPolicy,
                  (MODULE_ENUM(Chrono, Calendar, ReformId) id,
                   MODULE_TYPE(Chrono, CalendarPolicy) *outPolicy)),
        PREDICATE(IsLeapYear,
                  (MODULE_ENUM(Chrono, Calendar, System) system, int32_t year)),
        OPERATION(DaysInMonth,
                  (MODULE_ENUM(Chrono, Calendar, System) system, int32_t year,
                   uint8_t month, uint8_t *outDays)),
        OPERATION(DaysInYear,
                  (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                   int32_t year, uint16_t *outDays)),
        VARIANT(DayOfYear,
                OPERATION((const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                           MODULE_TYPE(Chrono, Date) date,
                           uint16_t *outDayOfYear)),
                OPERATION(Cumulative,
                          (MODULE_ENUM(Chrono, Calendar, System) system,
                           MODULE_TYPE(Chrono, Date) date,
                           uint16_t *outDayOfYear))),

        SPACE(
            Date,
            COMPARISON(Compare,
                       (MODULE_TYPE(Chrono, Date) left,
                        MODULE_TYPE(Chrono, Date) right)),
            PREDICATE(IsValid,
                      (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                       MODULE_TYPE(Chrono, Date) date)),
            OPERATION(ToDayOfYear,
                      (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                       MODULE_TYPE(Chrono, Date) date, uint16_t *outDayOfYear)),
            OPERATION(ToOrdinal,
                      (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                       MODULE_TYPE(Chrono, Date) date, int64_t *outOrdinal)),
            OPERATION(FromOrdinal,
                      (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                       int64_t ordinal, MODULE_TYPE(Chrono, Date) *outDate)),
            OPERATION(ToJDN,
                      (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                       MODULE_TYPE(Chrono, Date) date, int64_t *outJdn)),
            OPERATION(FromJDN,
                      (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                       int64_t jdn, MODULE_TYPE(Chrono, Date) *outDate)),
            OPERATION(ToRataDie,
                      (MODULE_TYPE(Chrono, Date) date, int64_t *outRataDie)),
            OPERATION(FromRataDie,
                      (int64_t rataDie, MODULE_TYPE(Chrono, Date) *outDate)),
            OPERATION(Difference,
                      (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                       MODULE_TYPE(Chrono, Date) left,
                       MODULE_TYPE(Chrono, Date) right, int64_t *outDays)),
            OPERATION(DayOfWeek,
                      (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                       MODULE_TYPE(Chrono, Date) date,
                       MODULE_ENUM(Chrono, Calendar, Weekday) *outWeekday)),
            OPERATION(FromNthWeekday,
                      (MODULE_ENUM(Chrono, Calendar, System) system,
                       int32_t year, uint8_t month,
                       MODULE_ENUM(Chrono, Calendar, WeekdayOccurrence)
                           occurrence,
                       MODULE_ENUM(Chrono, Calendar, Weekday) weekday,
                       MODULE_TYPE(Chrono, Date) *outDate)),
            OPERATION(FromWeekdayAnchor,
                      (MODULE_ENUM(Chrono, Calendar, System) system,
                       int32_t year, uint8_t month,
                       MODULE_ENUM(Chrono, Calendar, WeekdayAnchor) anchorKind,
                       uint8_t anchorDay,
                       MODULE_ENUM(Chrono, Calendar, Weekday) weekday,
                       MODULE_TYPE(Chrono, Date) *outDate)),
            OPERATION(AddDays,
                      (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                       MODULE_TYPE(Chrono, Date) date, int64_t days,
                       MODULE_TYPE(Chrono, Date) *outDate)),
            OPERATION(AddMonths,
                      (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                       MODULE_TYPE(Chrono, Date) date, int32_t months,
                       MODULE_TYPE(Chrono, Date) *outDate)),
            OPERATION(AddYears,
                      (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                       MODULE_TYPE(Chrono, Date) date, int32_t years,
                       MODULE_TYPE(Chrono, Date) *outDate)),
            OPERATION(ToIsoWeek,
                      (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                       MODULE_TYPE(Chrono, Date) date,
                       MODULE_TYPE(Chrono, DateIsoWeek) *outIsoWeek)),
            OPERATION(FromIsoWeek,
                      (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                       MODULE_TYPE(Chrono, DateIsoWeek) isoWeek,
                       MODULE_TYPE(Chrono, Date) *outDate)),
            OPERATION(Quarter,
                      (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                       MODULE_TYPE(Chrono, Date) date, uint8_t *outQuarter)),
            OPERATION(DaysRemainingInYear,
                      (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                       MODULE_TYPE(Chrono, Date) date, uint16_t *outDays)),
            PREDICATE(IsEndOfMonth,
                      (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                       MODULE_TYPE(Chrono, Date) date)),
            PREDICATE(IsWeekend,
                      (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                       MODULE_TYPE(Chrono, Date) date)),
            OPERATION(NextBusinessDay,
                      (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                       MODULE_TYPE(Chrono, Date) date,
                       MODULE_TYPE(Chrono, Date) *outDate)),
            OPERATION(AddBusinessDays,
                      (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                       MODULE_TYPE(Chrono, Date) date, int64_t businessDays,
                       MODULE_TYPE(Chrono, Date) *outDate)),
            OPERATION(BusinessDaysBetween,
                      (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                       MODULE_TYPE(Chrono, Date) start,
                       MODULE_TYPE(Chrono, Date) end, int64_t *outDays)))));

#undef COSMERON_MACRO_INTERNAL_CHRONO_REFORM_DISP
#undef COSMERON_MACRO_INTERNAL_CHRONO_MONTH_LEN_DISP
#undef COSMERON_MACRO_INTERNAL_CHRONO_MONTH_DISP

#include "Impl/Calendar.impl"

/* EOF */
