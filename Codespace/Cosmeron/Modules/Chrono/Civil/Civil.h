#pragma once

#include "../Calendar/Calendar.h"
#include "../Clock/Clock.h"

/* ========================================================================== */
/* Declarative Structs & Civil Package                                        */
/* ========================================================================== */

MODULE(DOMAIN(Modules), Chrono,
    STRUCT(TimeOfDay,
        FIELDS(
            uint16_t millisecond : 10;
            uint8_t second : 6;
            uint8_t minute : 6;
            uint8_t hour : 5;
        )
    ),

    STRUCT(CivilStamp,
        FIELDS(
            MODULE_TYPE(Chrono, Date) date;
            MODULE_TYPE(Chrono, TimeOfDay) time;
        )
    ),

    STRUCT(UtcOffset,
        FIELDS(
            int16_t minutesEastOfUtc;
        )
    ),

    STRUCT(CivilOffset,
        FIELDS(
            MODULE_TYPE(Chrono, CivilStamp) local;
            MODULE_TYPE(Chrono, UtcOffset) offset;
            bool daylightSavings : 1;
        )
    ),

    PACKAGE(Civil,
        PREDICATE(IsValid,
                  (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                   MODULE_TYPE(Chrono, CivilStamp) value)),
        COMPARISON(Compare,
                   (MODULE_TYPE(Chrono, CivilStamp) left,
                    MODULE_TYPE(Chrono, CivilStamp) right)),
        OPERATION(AddSpan,
                  (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                   MODULE_TYPE(Chrono, CivilStamp) value,
                   MODULE_TYPE(Chrono, Span) span,
                   MODULE_TYPE(Chrono, CivilStamp) *outResult)),
        OPERATION(Difference,
                  (const MODULE_TYPE(Chrono, CalendarPolicy) *policy,
                   MODULE_TYPE(Chrono, CivilStamp) left,
                   MODULE_TYPE(Chrono, CivilStamp) right,
                   MODULE_TYPE(Chrono, Span) *outResult)),
        OPERATION(ToInstantUTC,
                  (MODULE_TYPE(Chrono, CivilStamp) value,
                   MODULE_TYPE(Chrono, Instant) *outInstant)),
        OPERATION(FromInstantUTC,
                  (MODULE_TYPE(Chrono, Instant) instant,
                   MODULE_TYPE(Chrono, CivilStamp) *outValue)),
        FUNCTION(MODULE_TYPE(Chrono, CivilStamp), StartOfDay,
                 (MODULE_TYPE(Chrono, Date) date)),
        FUNCTION(MODULE_TYPE(Chrono, CivilStamp), Midday,
                 (MODULE_TYPE(Chrono, Date) date)),
        FUNCTION(MODULE_TYPE(Chrono, CivilStamp), EndOfDay,
                 (MODULE_TYPE(Chrono, Date) date)),
        FUNCTION(MODULE_TYPE(Chrono, CivilStamp), TruncateToHour,
                 (MODULE_TYPE(Chrono, CivilStamp) value)),
        FUNCTION(MODULE_TYPE(Chrono, CivilStamp), TruncateToMinute,
                 (MODULE_TYPE(Chrono, CivilStamp) value)),
        FUNCTION(MODULE_TYPE(Chrono, CivilStamp), TruncateToSecond,
                 (MODULE_TYPE(Chrono, CivilStamp) value)),

        SPACE(Time,
              PREDICATE(IsValid, (MODULE_TYPE(Chrono, TimeOfDay) time)),
              COMPARISON(Compare,
                         (MODULE_TYPE(Chrono, TimeOfDay) left,
                          MODULE_TYPE(Chrono, TimeOfDay) right)),
              OPERATION(ToMilliseconds,
                        (MODULE_TYPE(Chrono, TimeOfDay) time,
                         int64_t *outMillis)),
              OPERATION(FromMilliseconds,
                        (int64_t millis,
                         MODULE_TYPE(Chrono, TimeOfDay) *outTime)),
              OPERATION(AddSpan,
                        (MODULE_TYPE(Chrono, TimeOfDay) time,
                         MODULE_TYPE(Chrono, Span) span,
                         MODULE_TYPE(Chrono, TimeOfDay) *outTime,
                         int64_t *outDayCarry))
        )
    )
);

#include "Impl/Civil.impl"

/* EOF */
