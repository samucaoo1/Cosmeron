#pragma once

#include "../../../Core/Preprocessor/Detect/Detect.h"
#include "../Duration/Duration.h"

/* ========================================================================== */
/* Declarative Struct & Clock Package                                         */
/* ========================================================================== */

MODULE(DOMAIN(Modules), Chronometry,
    STRUCT(ClockTimePoint,
        FIELDS(
            uint64_t nanoseconds;
        )
    ),

    PACKAGE(Clock,
        OPERATION(Now, (MODULE_TYPE(Chronometry, ClockTimePoint) *outResult)),
        OPERATION(DurationBetween,
                  (MODULE_TYPE(Chronometry, ClockTimePoint) start,
                   MODULE_TYPE(Chronometry, ClockTimePoint) end,
                   MODULE_TYPE(Chronometry, Duration) *outResult)),
        OPERATION(Sleep, (MODULE_TYPE(Chronometry, Duration) duration))
    )
);

#include "Impl/Clock.impl"

/* EOF */
