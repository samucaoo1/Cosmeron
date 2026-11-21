#pragma once

#include "../Timestamp/Timestamp.h"
#include "../Duration/Duration.h"
#include "../Instant/Instant.h"

/* ========================================================================== */
/* Declarative Epoch Package                                                  */
/* ========================================================================== */

MODULE(DOMAIN(Modules), Chronometry,
    PACKAGE(Epoch,
        OPERATION(DateToJulianDayNumber,
                  (const MODULE_TYPE(Chronometry, CalendarPolicy) *policy,
                   MODULE_TYPE(Chronometry, Date) date,
                   int64_t *julianDayNumber)),
        OPERATION(DateFromJulianDayNumber,
                  (const MODULE_TYPE(Chronometry, CalendarPolicy) *policy,
                   int64_t julianDayNumber,
                   MODULE_TYPE(Chronometry, Date) *date)),
        OPERATION(DateToRataDie,
                  (MODULE_TYPE(Chronometry, Date) date, int64_t *rataDie)),
        OPERATION(DateFromRataDie,
                  (int64_t rataDie, MODULE_TYPE(Chronometry, Date) *date)),
        OPERATION(TimestampToUnix,
                  (MODULE_TYPE(Chronometry, Timestamp) value,
                   MODULE_TYPE(Chronometry, Instant) *instant)),
        OPERATION(TimestampFromUnix,
                  (MODULE_TYPE(Chronometry, Instant) instant,
                   MODULE_TYPE(Chronometry, Timestamp) *value)),
        OPERATION(TaiMinusUtcSeconds,
                  (MODULE_TYPE(Chronometry, Instant) utc,
                   int32_t *outSeconds)),
        OPERATION(UtcToTai,
                  (MODULE_TYPE(Chronometry, Instant) utc,
                   MODULE_TYPE(Chronometry, Instant) *outTai)),
        OPERATION(TaiToUtc,
                  (MODULE_TYPE(Chronometry, Instant) tai,
                   MODULE_TYPE(Chronometry, Instant) *outUtc))
    )
);

#include "Impl/Epoch.impl"

/* EOF */
