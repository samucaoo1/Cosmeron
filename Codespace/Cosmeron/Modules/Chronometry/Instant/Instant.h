#pragma once

#include "../Duration/Duration.h"

/* ========================================================================== */
/* Declarative Struct & Instant Package                                       */
/* ========================================================================== */

MODULE(DOMAIN(Modules), Chronometry,
    STRUCT(Instant,
        FIELDS(
            int64_t seconds;
            uint32_t nanoseconds;
        )
    ),

    PACKAGE(Instant,
        PREDICATE(IsValid, (MODULE_TYPE(Chronometry, Instant) instant)),
        COMPARISON(Compare,
                   (MODULE_TYPE(Chronometry, Instant) left,
                    MODULE_TYPE(Chronometry, Instant) right)),
        OPERATION(AddDuration,
                  (MODULE_TYPE(Chronometry, Instant) instant,
                   MODULE_TYPE(Chronometry, Duration) duration,
                   MODULE_TYPE(Chronometry, Instant) *outResult)),
        OPERATION(DurationBetween,
                  (MODULE_TYPE(Chronometry, Instant) start,
                   MODULE_TYPE(Chronometry, Instant) end,
                   MODULE_TYPE(Chronometry, Duration) *outResult))
    )
);

#include "Impl/Instant.impl"

/* EOF */
