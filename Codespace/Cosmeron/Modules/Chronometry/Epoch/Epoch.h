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
        OPERATION(DateTimeToUnix,
                  (MODULE_TYPE(Chronometry, TimeStamp) value,
                   MODULE_TYPE(Chronometry, Instant) *instant)),
        OPERATION(DateTimeFromUnix,
                  (MODULE_TYPE(Chronometry, Instant) instant,
                   MODULE_TYPE(Chronometry, TimeStamp) *value))
    )
);

#include "Impl/Epoch.impl"

/* EOF */
