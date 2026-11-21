#pragma once

#include "../Timestamp/Timestamp.h"
#include "../Duration/Duration.h"
#include "../Instant/Instant.h"

/* ========================================================================== */
/* Declarative Epoch Package                                                  */
/* ========================================================================== */

MODULE(DOMAIN(Modules), Chronometry,
    PACKAGE(Epoch,
        VARIANT(DateTo,
                OPERATION(JulianDayNumber,
                          (const MODULE_TYPE(Chronometry, CalendarPolicy)
                               *policy,
                           MODULE_TYPE(Chronometry, Date) date,
                           int64_t *julianDayNumber)),
                OPERATION(RataDie,
                          (MODULE_TYPE(Chronometry, Date) date,
                           int64_t *rataDie))
        ),

        VARIANT(DateFrom,
                OPERATION(JulianDayNumber,
                          (const MODULE_TYPE(Chronometry, CalendarPolicy)
                               *policy,
                           int64_t julianDayNumber,
                           MODULE_TYPE(Chronometry, Date) *date)),
                OPERATION(RataDie,
                          (int64_t rataDie,
                           MODULE_TYPE(Chronometry, Date) *date))
        ),

        VARIANT(TimestampTo,
                OPERATION(Unix,
                          (MODULE_TYPE(Chronometry, Timestamp) value,
                           MODULE_TYPE(Chronometry, Instant) *instant))
        ),

        VARIANT(TimestampFrom,
                OPERATION(Unix,
                          (MODULE_TYPE(Chronometry, Instant) instant,
                           MODULE_TYPE(Chronometry, Timestamp) *value))
        ),

        VARIANT(TAI,
                OPERATION(MinusUtcSeconds,
                          (MODULE_TYPE(Chronometry, Instant) utc,
                           int32_t *outSeconds)),
                OPERATION(FromUtc,
                          (MODULE_TYPE(Chronometry, Instant) utc,
                           MODULE_TYPE(Chronometry, Instant) *outTai)),
                OPERATION(ToUtc,
                          (MODULE_TYPE(Chronometry, Instant) tai,
                           MODULE_TYPE(Chronometry, Instant) *outUtc))
        )
    )
);

#include "Impl/Epoch.impl"

/* EOF */