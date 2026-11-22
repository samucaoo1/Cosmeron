#pragma once

#include "../Civil/Civil.h"
#include "Impl/IanaZones.inc"

/* ========================================================================== */
/* 1. Decoupled X-Macro IANA Table Selectors & Time Base Table                */
/* ========================================================================== */

#define CHRONO_TIMEZONE_IANA_COL_ID(                                           \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  ID
#define CHRONO_TIMEZONE_IANA_COL_IANA_NAME(                                    \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  IANA_NAME
#define CHRONO_TIMEZONE_IANA_COL_STD_OFFSET_MIN(                               \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  STD_OFFSET_MIN
#define CHRONO_TIMEZONE_IANA_COL_DST_SAVINGS_MIN(                              \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  DST_SAVINGS_MIN
#define CHRONO_TIMEZONE_IANA_COL_START_MONTH(                                  \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  START_MONTH
#define CHRONO_TIMEZONE_IANA_COL_START_ANCHOR_KIND(                            \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  START_ANCHOR_KIND
#define CHRONO_TIMEZONE_IANA_COL_START_ANCHOR_DAY(                             \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  START_ANCHOR_DAY
#define CHRONO_TIMEZONE_IANA_COL_START_WEEKDAY(                                \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  START_WEEKDAY
#define CHRONO_TIMEZONE_IANA_COL_START_HOUR(                                   \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  START_HOUR
#define CHRONO_TIMEZONE_IANA_COL_START_MINUTE(                                 \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  START_MINUTE
#define CHRONO_TIMEZONE_IANA_COL_START_BASE(                                   \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  START_BASE
#define CHRONO_TIMEZONE_IANA_COL_END_MONTH(                                    \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  END_MONTH
#define CHRONO_TIMEZONE_IANA_COL_END_ANCHOR_KIND(                              \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  END_ANCHOR_KIND
#define CHRONO_TIMEZONE_IANA_COL_END_ANCHOR_DAY(                               \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  END_ANCHOR_DAY
#define CHRONO_TIMEZONE_IANA_COL_END_WEEKDAY(                                  \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  END_WEEKDAY
#define CHRONO_TIMEZONE_IANA_COL_END_HOUR(                                     \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  END_HOUR
#define CHRONO_TIMEZONE_IANA_COL_END_MINUTE(                                   \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  END_MINUTE
#define CHRONO_TIMEZONE_IANA_COL_END_BASE(                                     \
    ID, IANA_NAME, STD_OFFSET_MIN, DST_SAVINGS_MIN, START_MONTH,               \
    START_ANCHOR_KIND, START_ANCHOR_DAY, START_WEEKDAY, START_HOUR,            \
    START_MINUTE, START_BASE, END_MONTH, END_ANCHOR_KIND, END_ANCHOR_DAY,      \
    END_WEEKDAY, END_HOUR, END_MINUTE, END_BASE)                               \
  END_BASE
#define CHRONO_TIMEZONE_IANA_GET(FIELD, ROW)                                   \
  PP_OP_CAT2(CHRONO_TIMEZONE_IANA_COL_, FIELD) ROW

#define CHRONO_TIMEZONE_LINK_COL_ALIAS(ALIAS, TARGET) ALIAS
#define CHRONO_TIMEZONE_LINK_COL_TARGET(ALIAS, TARGET) TARGET
#define CHRONO_TIMEZONE_LINK_GET(FIELD, ROW)                                   \
  PP_OP_CAT2(CHRONO_TIMEZONE_LINK_COL_, FIELD) ROW

#define CHRONO_TIMEZONE_COUNTRY_COL_CODE(CODE, NAME) CODE
#define CHRONO_TIMEZONE_COUNTRY_COL_NAME(CODE, NAME) NAME
#define CHRONO_TIMEZONE_COUNTRY_GET(FIELD, ROW)                                \
  PP_OP_CAT2(CHRONO_TIMEZONE_COUNTRY_COL_, FIELD) ROW

#define CHRONO_TIMEZONE_COUNTRY_ZONE_COL_CODE(CODE, ZONE_ID) CODE
#define CHRONO_TIMEZONE_COUNTRY_ZONE_COL_ZONE_ID(CODE, ZONE_ID) ZONE_ID
#define CHRONO_TIMEZONE_COUNTRY_ZONE_GET(FIELD, ROW)                           \
  PP_OP_CAT2(CHRONO_TIMEZONE_COUNTRY_ZONE_COL_, FIELD) ROW

#define CHRONO_TIMEZONE_LOCAL_STATUS_COL_NAME(NAME) NAME
#define CHRONO_TIMEZONE_LOCAL_STATUS_GET(FIELD, ROW)                           \
  PP_OP_CAT2(CHRONO_TIMEZONE_LOCAL_STATUS_COL_, FIELD) ROW

#define CHRONO_TIMEZONE_LOCAL_STATUS_TABLE(X)                                  \
  X((LOCAL_TIME_UNIQUE))                                                       \
  X((LOCAL_TIME_AMBIGUOUS))                                                    \
  X((LOCAL_TIME_NONEXISTENT))

#define CHRONO_TIMEZONE_DISAMBIGUATION_COL_NAME(NAME) NAME
#define CHRONO_TIMEZONE_DISAMBIGUATION_GET(FIELD, ROW)                         \
  PP_OP_CAT2(CHRONO_TIMEZONE_DISAMBIGUATION_COL_, FIELD) ROW

#define CHRONO_TIMEZONE_DISAMBIGUATION_TABLE(X)                                \
  X((DISAMBIGUATE_COMPATIBLE))                                                 \
  X((DISAMBIGUATE_EARLIER))                                                    \
  X((DISAMBIGUATE_LATER))                                                      \
  X((DISAMBIGUATE_REJECT))

#define CHRONO_TIMEZONE_TIME_BASE_COL_NAME(NAME) NAME
#define CHRONO_TIMEZONE_TIME_BASE_GET(FIELD, ROW)                              \
  PP_OP_CAT2(CHRONO_TIMEZONE_TIME_BASE_COL_, FIELD) ROW

#define CHRONO_TIMEZONE_TIME_BASE_TABLE(X)                                     \
  X((TIME_BASE_WALL))                                                          \
  X((TIME_BASE_STANDARD))                                                      \
  X((TIME_BASE_UTC))

#define COSMERON_MACRO_INTERNAL_CHRONO_TBASE_DISP(TARGET, ENUM_NAME, ROW)      \
  CNS(TARGET, CHRONO_TIMEZONE_TIME_BASE_GET(NAME, ROW)),

#define COSMERON_MACRO_INTERNAL_CHRONO_LSTAT_DISP(TARGET, ENUM_NAME, ROW)      \
  CNS(TARGET, CHRONO_TIMEZONE_LOCAL_STATUS_GET(NAME, ROW)),

#define COSMERON_MACRO_INTERNAL_CHRONO_DISAM_DISP(TARGET, ENUM_NAME, ROW)      \
  CNS(TARGET, CHRONO_TIMEZONE_DISAMBIGUATION_GET(NAME, ROW)),

#define COSMERON_MACRO_INTERNAL_CHRONO_ZONE_DISP(ROW)                          \
  CNS(MODULE_NS(Chrono, Timezone), CHRONO_TIMEZONE_IANA_GET(ID, ROW)),

ENUM_DEF_EX(MODULE_NS(Chrono, Timezone), TransitionTimeBase,
            CHRONO_TIMEZONE_TIME_BASE_TABLE,
            COSMERON_MACRO_INTERNAL_CHRONO_TBASE_DISP)
ENUM_DEF_EX(MODULE_NS(Chrono, Timezone), LocalTimeStatus,
            CHRONO_TIMEZONE_LOCAL_STATUS_TABLE,
            COSMERON_MACRO_INTERNAL_CHRONO_LSTAT_DISP)
ENUM_DEF_EX(MODULE_NS(Chrono, Timezone), Disambiguation,
            CHRONO_TIMEZONE_DISAMBIGUATION_TABLE,
            COSMERON_MACRO_INTERNAL_CHRONO_DISAM_DISP)
ENUM_DEF_DIRECT(MODULE_NS(Chrono, Timezone), ZoneId, CHRONO_TIMEZONE_IANA_TABLE,
                COSMERON_MACRO_INTERNAL_CHRONO_ZONE_DISP)

#undef COSMERON_MACRO_INTERNAL_CHRONO_ZONE_DISP
#undef COSMERON_MACRO_INTERNAL_CHRONO_DISAM_DISP
#undef COSMERON_MACRO_INTERNAL_CHRONO_LSTAT_DISP
#undef COSMERON_MACRO_INTERNAL_CHRONO_TBASE_DISP

/* ========================================================================== */
/* 2. Declarative Structs & Timezone Package                                  */
/* ========================================================================== */

MODULE(DOMAIN(Modules), Chrono,
    STRUCT(TimezoneDstRule,
        FIELDS(
            uint8_t month;
            MODULE_ENUM(Chrono, Calendar, WeekdayAnchor) anchorKind;
            uint8_t anchorDay;
            MODULE_ENUM(Chrono, Calendar, Weekday) weekday;
            uint8_t localHour;
            uint8_t localMinute;
            MODULE_ENUM(Chrono, Timezone, TransitionTimeBase) timeBase;
        )
    ),

    STRUCT(TimezoneRule,
        FIELDS(
            const char *ianaName;
            MODULE_TYPE(Chrono, UtcOffset) standardOffset;
            int16_t dstSavingsMinutes;
            MODULE_TYPE(Chrono, TimezoneDstRule) start;
            MODULE_TYPE(Chrono, TimezoneDstRule) end;
        )
    ),

    PACKAGE(Timezone,
        OPERATION(ToUTC,
                  (MODULE_TYPE(Chrono, CivilOffset) value,
                   MODULE_TYPE(Chrono, Instant) *instant)),
        VARIANT(FromUTC,
                OPERATION((MODULE_TYPE(Chrono, Instant) instant,
                           MODULE_TYPE(Chrono, UtcOffset) offset,
                           MODULE_TYPE(Chrono, CivilOffset) *value)),
                OPERATION(WithRule,
                          (MODULE_TYPE(Chrono, Instant) instant,
                           const MODULE_TYPE(Chrono, TimezoneRule) *rule,
                           MODULE_TYPE(Chrono, CivilOffset) *value)),
                OPERATION(WithZone,
                          (MODULE_TYPE(Chrono, Instant) instant,
                           MODULE_ENUM(Chrono, Timezone, ZoneId) zoneId,
                           MODULE_TYPE(Chrono, CivilOffset) *value))
        ),
        OPERATION(ConvertRule,
                  (MODULE_TYPE(Chrono, CivilOffset) value,
                   const MODULE_TYPE(Chrono, TimezoneRule) *targetRule,
                   MODULE_TYPE(Chrono, CivilOffset) *outConverted)),
        OPERATION(ConvertZone,
                  (MODULE_TYPE(Chrono, CivilOffset) value,
                   MODULE_ENUM(Chrono, Timezone, ZoneId) targetZone,
                   MODULE_TYPE(Chrono, CivilOffset) *outConverted)),
        OPERATION(SystemOffsetAt,
                  (MODULE_TYPE(Chrono, Instant) instant,
                   MODULE_TYPE(Chrono, UtcOffset) *outOffset,
                   bool *outIsDst)),
        OPERATION(SystemNow,
                  (MODULE_TYPE(Chrono, CivilOffset) *outValue)),

        SPACE(Offset,
              PREDICATE(IsValid, (MODULE_TYPE(Chrono, UtcOffset) offset)),
              OPERATION(FromHoursMinutes,
                        (int8_t hours, int8_t minutes,
                         MODULE_TYPE(Chrono, UtcOffset) *outOffset))
        ),

        SPACE(Zone,
              FUNCTION(const char *, DataVersion, ()),
              OPERATION(FromId,
                        (MODULE_ENUM(Chrono, Timezone, ZoneId) zoneId,
                         MODULE_TYPE(Chrono, TimezoneRule) *outRule)),
              OPERATION(FromName,
                        (const char *ianaName,
                         MODULE_TYPE(Chrono, TimezoneRule) *outRule)),
              PREDICATE(IsRuleValid,
                        (const MODULE_TYPE(Chrono, TimezoneRule) *rule)),
              OPERATION(OffsetAt,
                        (const MODULE_TYPE(Chrono, TimezoneRule) *rule,
                         MODULE_TYPE(Chrono, Instant) instant,
                         MODULE_TYPE(Chrono, UtcOffset) *outOffset,
                         bool *outIsDst)),
              OPERATION(NextTransition,
                        (const MODULE_TYPE(Chrono, TimezoneRule) *rule,
                         MODULE_TYPE(Chrono, Instant) instant,
                         MODULE_TYPE(Chrono, Instant) *outTransition,
                         MODULE_TYPE(Chrono, UtcOffset) *outNewOffset)),
              OPERATION(PreviousTransition,
                        (const MODULE_TYPE(Chrono, TimezoneRule) *rule,
                         MODULE_TYPE(Chrono, Instant) instant,
                         MODULE_TYPE(Chrono, Instant) *outTransition,
                         MODULE_TYPE(Chrono, UtcOffset) *outNewOffset)),
              OPERATION(IsDstAtLocal,
                        (const MODULE_TYPE(Chrono, TimezoneRule) *rule,
                         MODULE_TYPE(Chrono, CivilStamp) local,
                         bool *outIsDst)),
              OPERATION(ClassifyLocal,
                        (const MODULE_TYPE(Chrono, TimezoneRule) *rule,
                         MODULE_TYPE(Chrono, CivilStamp) local,
                         MODULE_ENUM(Chrono, Timezone, LocalTimeStatus)
                             *outStatus,
                         MODULE_TYPE(Chrono, CivilOffset) *outEarlier,
                         MODULE_TYPE(Chrono, CivilOffset) *outLater)),
              OPERATION(ResolveLocal,
                        (const MODULE_TYPE(Chrono, TimezoneRule) *rule,
                         MODULE_TYPE(Chrono, CivilStamp) local,
                         MODULE_TYPE(Chrono, CivilOffset) *outResolved)),
              OPERATION(ResolveLocalWithPolicy,
                        (const MODULE_TYPE(Chrono, TimezoneRule) *rule,
                         MODULE_TYPE(Chrono, CivilStamp) local,
                         MODULE_ENUM(Chrono, Timezone, Disambiguation) policy,
                         MODULE_TYPE(Chrono, CivilOffset) *outResolved))
        ),

        SPACE(Country,
              OPERATION(NameFromCode,
                        (const char *countryCode,
                         const char **outCountryName)),
              OPERATION(ZoneCount,
                        (const char *countryCode, size_t *outCount)),
              OPERATION(ZoneIdAt,
                        (const char *countryCode, size_t index,
                         MODULE_ENUM(Chrono, Timezone, ZoneId) *outZoneId)),
              OPERATION(ZoneRuleAt,
                        (const char *countryCode, size_t index,
                         MODULE_TYPE(Chrono, TimezoneRule) *outRule))
        )
    )
);

#include "Impl/Timezone.impl"

/* EOF */
