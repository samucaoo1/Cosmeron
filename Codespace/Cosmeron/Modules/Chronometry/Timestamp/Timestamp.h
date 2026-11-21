#pragma once

#include "../Calendar/Calendar.h"
#include "../Duration/Duration.h"

/* ========================================================================== */
/* Declarative Structs & Timestamp Package                                    */
/* ========================================================================== */

MODULE(DOMAIN(Modules), Chronometry,
    STRUCT(TimeOfDay,
        FIELDS(
            uint16_t millisecond : 10;
            uint8_t second : 6;
            uint8_t minute : 6;
            uint8_t hour : 5;
        )
    ),

    STRUCT(TimeStamp,
        FIELDS(
            MODULE_TYPE(Chronometry, Date) date;
            MODULE_TYPE(Chronometry, TimeOfDay) time;
        )
    ),

    PACKAGE(Timestamp,
        PREDICATE(IsValid,
                  (const MODULE_TYPE(Chronometry, CalendarPolicy) *policy,
                   MODULE_TYPE(Chronometry, TimeStamp) value)),
        COMPARISON(Compare,
                   (MODULE_TYPE(Chronometry, TimeStamp) left,
                    MODULE_TYPE(Chronometry, TimeStamp) right)),
        OPERATION(AddDuration,
                  (const MODULE_TYPE(Chronometry, CalendarPolicy) *policy,
                   MODULE_TYPE(Chronometry, TimeStamp) value,
                   MODULE_TYPE(Chronometry, Duration) duration,
                   MODULE_TYPE(Chronometry, TimeStamp) *outResult)),
        OPERATION(Difference,
                  (const MODULE_TYPE(Chronometry, CalendarPolicy) *policy,
                   MODULE_TYPE(Chronometry, TimeStamp) left,
                   MODULE_TYPE(Chronometry, TimeStamp) right,
                   MODULE_TYPE(Chronometry, Duration) *outResult)),

        SPACE(Time,
              PREDICATE(IsValid, (MODULE_TYPE(Chronometry, TimeOfDay) time)),
              COMPARISON(Compare,
                         (MODULE_TYPE(Chronometry, TimeOfDay) left,
                          MODULE_TYPE(Chronometry, TimeOfDay) right))
        )
    )
);

#include "Impl/Timestamp.impl"

/* EOF */
