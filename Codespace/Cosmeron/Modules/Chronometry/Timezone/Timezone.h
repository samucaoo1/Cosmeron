#pragma once

#include "../Epoch/Epoch.h"

/* ========================================================================== */
/* Declarative Structs & Timezone Package                                     */
/* ========================================================================== */

MODULE(DOMAIN(Modules), Chronometry,
    STRUCT(UtcOffset,
        FIELDS(
            int16_t minutesEastOfUtc;
        )
    ),

    STRUCT(OffsetDateTime,
        FIELDS(
            MODULE_TYPE(Chronometry, TimeStamp) local;
            MODULE_TYPE(Chronometry, UtcOffset) offset;
            bool daylightSavings : 1;
        )
    ),

    PACKAGE(Timezone,
        OPERATION(ToUTC,
                  (MODULE_TYPE(Chronometry, OffsetDateTime) value,
                   MODULE_TYPE(Chronometry, Instant) *instant)),
        OPERATION(FromUTC,
                  (MODULE_TYPE(Chronometry, Instant) instant,
                   MODULE_TYPE(Chronometry, UtcOffset) offset,
                   MODULE_TYPE(Chronometry, OffsetDateTime) *value)),

        SPACE(Offset,
              PREDICATE(IsValid, (MODULE_TYPE(Chronometry, UtcOffset) offset))
        )
    )
);

#include "Impl/Timezone.impl"

/* EOF */
