#pragma once

#include "../Epoch/Epoch.h"
#include "Impl/IanaZones.inc"

/* ========================================================================== */
/* 1. Decoupled X-Macro IANA Table Selectors & Time Base Table                */
/* ========================================================================== */

#define CHRONOMETRY_TIMEZONE_IANA_COL_ID(                                      \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  ID
#define CHRONOMETRY_TIMEZONE_IANA_COL_IANA_NAME(                               \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  IANA_NAME
#define CHRONOMETRY_TIMEZONE_IANA_COL_STD_OFFSET_MIN(                          \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  STD_OFFSET_MIN
#define CHRONOMETRY_TIMEZONE_IANA_COL_DST_SAVINGS_MIN(                         \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  DST_SAVINGS_MIN
#define CHRONOMETRY_TIMEZONE_IANA_COL_START_MONTH(                             \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  START_MONTH
#define CHRONOMETRY_TIMEZONE_IANA_COL_START_ANCHOR_KIND(                       \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  START_ANCHOR_KIND
#define CHRONOMETRY_TIMEZONE_IANA_COL_START_ANCHOR_DAY(                        \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  START_ANCHOR_DAY
#define CHRONOMETRY_TIMEZONE_IANA_COL_START_WEEKDAY(                           \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  START_WEEKDAY
#define CHRONOMETRY_TIMEZONE_IANA_COL_START_HOUR(                              \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  START_HOUR
#define CHRONOMETRY_TIMEZONE_IANA_COL_START_MINUTE(                            \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  START_MINUTE
#define CHRONOMETRY_TIMEZONE_IANA_COL_START_BASE(                              \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  START_BASE
#define CHRONOMETRY_TIMEZONE_IANA_COL_END_MONTH(                               \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  END_MONTH
#define CHRONOMETRY_TIMEZONE_IANA_COL_END_ANCHOR_KIND(                         \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  END_ANCHOR_KIND
#define CHRONOMETRY_TIMEZONE_IANA_COL_END_ANCHOR_DAY(                          \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  END_ANCHOR_DAY
#define CHRONOMETRY_TIMEZONE_IANA_COL_END_WEEKDAY(                             \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  END_WEEKDAY
#define CHRONOMETRY_TIMEZONE_IANA_COL_END_HOUR(                                \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  END_HOUR
#define CHRONOMETRY_TIMEZONE_IANA_COL_END_MINUTE(                              \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  END_MINUTE
#define CHRONOMETRY_TIMEZONE_IANA_COL_END_BASE(                                \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  END_BASE
#define CHRONOMETRY_TIMEZONE_IANA_GET(FIELD, ROW)                              \
  PP_OP_CAT2(CHRONOMETRY_TIMEZONE_IANA_COL_, FIELD) ROW

#define CHRONOMETRY_TIMEZONE_LINK_COL_ALIAS(ALIAS, TARGET) ALIAS
#define CHRONOMETRY_TIMEZONE_LINK_COL_TARGET(ALIAS, TARGET) TARGET
#define CHRONOMETRY_TIMEZONE_LINK_GET(FIELD, ROW)                              \
  PP_OP_CAT2(CHRONOMETRY_TIMEZONE_LINK_COL_, FIELD) ROW

#define CHRONOMETRY_TIMEZONE_COUNTRY_COL_CODE(CODE, NAME) CODE
#define CHRONOMETRY_TIMEZONE_COUNTRY_COL_NAME(CODE, NAME) NAME
#define CHRONOMETRY_TIMEZONE_COUNTRY_GET(FIELD, ROW)                           \
  PP_OP_CAT2(CHRONOMETRY_TIMEZONE_COUNTRY_COL_, FIELD) ROW

#define CHRONOMETRY_TIMEZONE_COUNTRY_ZONE_COL_CODE(CODE, ZONE_ID) CODE
#define CHRONOMETRY_TIMEZONE_COUNTRY_ZONE_COL_ZONE_ID(CODE, ZONE_ID) ZONE_ID
#define CHRONOMETRY_TIMEZONE_COUNTRY_ZONE_GET(FIELD, ROW)                      \
  PP_OP_CAT2(CHRONOMETRY_TIMEZONE_COUNTRY_ZONE_COL_, FIELD) ROW

#define CHRONOMETRY_TIMEZONE_LOCAL_STATUS_COL_NAME(NAME) NAME
#define CHRONOMETRY_TIMEZONE_LOCAL_STATUS_GET(FIELD, ROW)                      \
  PP_OP_CAT2(CHRONOMETRY_TIMEZONE_LOCAL_STATUS_COL_, FIELD) ROW

#define CHRONOMETRY_TIMEZONE_LOCAL_STATUS_TABLE(X)                             \
  X((LOCAL_TIME_UNIQUE))                                                       \
  X((LOCAL_TIME_AMBIGUOUS))                                                    \
  X((LOCAL_TIME_NONEXISTENT))

#define CHRONOMETRY_TIMEZONE_DISAMBIGUATION_COL_NAME(NAME) NAME
#define CHRONOMETRY_TIMEZONE_DISAMBIGUATION_GET(FIELD, ROW)                    \
  PP_OP_CAT2(CHRONOMETRY_TIMEZONE_DISAMBIGUATION_COL_, FIELD) ROW

#define CHRONOMETRY_TIMEZONE_DISAMBIGUATION_TABLE(X)                           \
  X((DISAMBIGUATE_COMPATIBLE))                                                 \
  X((DISAMBIGUATE_EARLIER))                                                    \
  X((DISAMBIGUATE_LATER))                                                      \
  X((DISAMBIGUATE_REJECT))

#define CHRONOMETRY_TIMEZONE_TIME_BASE_COL_NAME(NAME) NAME
#define CHRONOMETRY_TIMEZONE_TIME_BASE_GET(FIELD, ROW)                         \
  PP_OP_CAT2(CHRONOMETRY_TIMEZONE_TIME_BASE_COL_, FIELD) ROW

#define CHRONOMETRY_TIMEZONE_TIME_BASE_TABLE(X)                                \
  X((TIME_BASE_WALL))                                                          \
  X((TIME_BASE_STANDARD))                                                      \
  X((TIME_BASE_UTC))

#define COSMERON_MACRO_INTERNAL_CHRONOMETRY_TBASE_DISP(TARGET, ENUM_NAME, ROW) \
  CNS(TARGET, CHRONOMETRY_TIMEZONE_TIME_BASE_GET(NAME, ROW)),

#define COSMERON_MACRO_INTERNAL_CHRONOMETRY_LSTAT_DISP(TARGET, ENUM_NAME, ROW) \
  CNS(TARGET, CHRONOMETRY_TIMEZONE_LOCAL_STATUS_GET(NAME, ROW)),

#define COSMERON_MACRO_INTERNAL_CHRONOMETRY_DISAM_DISP(TARGET, ENUM_NAME, ROW) \
  CNS(TARGET, CHRONOMETRY_TIMEZONE_DISAMBIGUATION_GET(NAME, ROW)),

#define COSMERON_MACRO_INTERNAL_CHRONOMETRY_ZONE_DISP(ROW)                     \
  CNS(MODULE_NS(Chronometry, Timezone),                                        \
      CHRONOMETRY_TIMEZONE_IANA_GET(ID, ROW)),

ENUM_DEF_EX(MODULE_NS(Chronometry, Timezone), TransitionTimeBase,
            CHRONOMETRY_TIMEZONE_TIME_BASE_TABLE,
            COSMERON_MACRO_INTERNAL_CHRONOMETRY_TBASE_DISP)
ENUM_DEF_EX(MODULE_NS(Chronometry, Timezone), LocalTimeStatus,
            CHRONOMETRY_TIMEZONE_LOCAL_STATUS_TABLE,
            COSMERON_MACRO_INTERNAL_CHRONOMETRY_LSTAT_DISP)
ENUM_DEF_EX(MODULE_NS(Chronometry, Timezone), Disambiguation,
            CHRONOMETRY_TIMEZONE_DISAMBIGUATION_TABLE,
            COSMERON_MACRO_INTERNAL_CHRONOMETRY_DISAM_DISP)
ENUM_DEF_DIRECT(MODULE_NS(Chronometry, Timezone), ZoneId,
                CHRONOMETRY_TIMEZONE_IANA_TABLE,
                COSMERON_MACRO_INTERNAL_CHRONOMETRY_ZONE_DISP)

#undef COSMERON_MACRO_INTERNAL_CHRONOMETRY_ZONE_DISP
#undef COSMERON_MACRO_INTERNAL_CHRONOMETRY_DISAM_DISP
#undef COSMERON_MACRO_INTERNAL_CHRONOMETRY_LSTAT_DISP
#undef COSMERON_MACRO_INTERNAL_CHRONOMETRY_TBASE_DISP

/* ========================================================================== */
/* 2. Declarative Structs & Timezone Package                                  */
/* ========================================================================== */

MODULE(DOMAIN(Modules), Chronometry,
    STRUCT(UtcOffset,
        FIELDS(
            int16_t minutesEastOfUtc;
        )
    ),

    STRUCT(OffsetTimestamp,
        FIELDS(
            MODULE_TYPE(Chronometry, Timestamp) local;
            MODULE_TYPE(Chronometry, UtcOffset) offset;
            bool daylightSavings : 1;
        )
    ),

    STRUCT(DstTransitionRule,
        FIELDS(
            uint8_t month;
            MODULE_ENUM(Chronometry, Calendar, WeekdayAnchor) anchorKind;
            uint8_t anchorDay;
            MODULE_ENUM(Chronometry, Calendar, Weekday) weekday;
            uint8_t localHour;
            uint8_t localMinute;
            MODULE_ENUM(Chronometry, Timezone, TransitionTimeBase) timeBase;
        )
    ),

    STRUCT(TimezoneRule,
        FIELDS(
            const char *ianaName;
            MODULE_TYPE(Chronometry, UtcOffset) standardOffset;
            int16_t dstSavingsMinutes;
            MODULE_TYPE(Chronometry, DstTransitionRule) start;
            MODULE_TYPE(Chronometry, DstTransitionRule) end;
        )
    ),

    PACKAGE(Timezone,
        OPERATION(ToUTC,
                  (MODULE_TYPE(Chronometry, OffsetTimestamp) value,
                   MODULE_TYPE(Chronometry, Instant) *instant)),
        VARIANT(FromUTC,
                OPERATION((MODULE_TYPE(Chronometry, Instant) instant,
                           MODULE_TYPE(Chronometry, UtcOffset) offset,
                           MODULE_TYPE(Chronometry, OffsetTimestamp) *value)),
                OPERATION(WithRule,
                          (MODULE_TYPE(Chronometry, Instant) instant,
                           const MODULE_TYPE(Chronometry, TimezoneRule) *rule,
                           MODULE_TYPE(Chronometry, OffsetTimestamp) *value)),
                OPERATION(WithZone,
                          (MODULE_TYPE(Chronometry, Instant) instant,
                           MODULE_ENUM(Chronometry, Timezone, ZoneId) zoneId,
                           MODULE_TYPE(Chronometry, OffsetTimestamp) *value))
        ),
        OPERATION(SystemOffsetAt,
                  (MODULE_TYPE(Chronometry, Instant) instant,
                   MODULE_TYPE(Chronometry, UtcOffset) *outOffset,
                   bool *outIsDst)),
        OPERATION(SystemNow,
                  (MODULE_TYPE(Chronometry, OffsetTimestamp) *outValue)),

        SPACE(Offset,
              PREDICATE(IsValid, (MODULE_TYPE(Chronometry, UtcOffset) offset)),
              OPERATION(FromHoursMinutes,
                        (int8_t hours, int8_t minutes,
                         MODULE_TYPE(Chronometry, UtcOffset) *outOffset))
        ),

        SPACE(Zone,
              FUNCTION(const char *, DataVersion, ()),
              OPERATION(FromId,
                        (MODULE_ENUM(Chronometry, Timezone, ZoneId) zoneId,
                         MODULE_TYPE(Chronometry, TimezoneRule) *outRule)),
              OPERATION(FromName,
                        (const char *ianaName,
                         MODULE_TYPE(Chronometry, TimezoneRule) *outRule)),
              PREDICATE(IsRuleValid,
                        (const MODULE_TYPE(Chronometry, TimezoneRule) *rule)),
              OPERATION(IsDstAtLocal,
                        (const MODULE_TYPE(Chronometry, TimezoneRule) *rule,
                         MODULE_TYPE(Chronometry, Timestamp) local,
                         bool *outIsDst)),
              OPERATION(ClassifyLocal,
                        (const MODULE_TYPE(Chronometry, TimezoneRule) *rule,
                         MODULE_TYPE(Chronometry, Timestamp) local,
                         MODULE_ENUM(Chronometry, Timezone, LocalTimeStatus)
                             *outStatus,
                         MODULE_TYPE(Chronometry, OffsetTimestamp) *outEarlier,
                         MODULE_TYPE(Chronometry, OffsetTimestamp) *outLater)),
              OPERATION(ResolveLocal,
                        (const MODULE_TYPE(Chronometry, TimezoneRule) *rule,
                         MODULE_TYPE(Chronometry, Timestamp) local,
                         MODULE_TYPE(Chronometry, OffsetTimestamp)
                             *outResolved)),
              OPERATION(ResolveLocalWithPolicy,
                        (const MODULE_TYPE(Chronometry, TimezoneRule) *rule,
                         MODULE_TYPE(Chronometry, Timestamp) local,
                         MODULE_ENUM(Chronometry, Timezone, Disambiguation)
                             policy,
                         MODULE_TYPE(Chronometry, OffsetTimestamp)
                             *outResolved))
        ),

        SPACE(Country,
              OPERATION(NameFromCode,
                        (const char *countryCode,
                         const char **outCountryName)),
              OPERATION(ZoneCount,
                        (const char *countryCode, size_t *outCount)),
              OPERATION(ZoneIdAt,
                        (const char *countryCode, size_t index,
                         MODULE_ENUM(Chronometry, Timezone, ZoneId)
                             *outZoneId)),
              OPERATION(ZoneRuleAt,
                        (const char *countryCode, size_t index,
                         MODULE_TYPE(Chronometry, TimezoneRule) *outRule))
        )
    )
);

#include "Impl/Timezone.impl"

/* EOF */