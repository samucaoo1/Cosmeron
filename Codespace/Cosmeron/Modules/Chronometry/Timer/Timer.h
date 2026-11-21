#pragma once

#include "../Clock/Clock.h"

/* ========================================================================== */
/* Declarative Structs & Timer Package                                        */
/* ========================================================================== */

MODULE(DOMAIN(Modules), Chronometry,
    STRUCT(Timer,
        FIELDS(
            MODULE_TYPE(Chronometry, ClockTimePoint) start;
        )
    ),

    STRUCT(Delta,
        FIELDS(
            MODULE_TYPE(Chronometry, ClockTimePoint) previous;
        )
    ),

    STRUCT(FrameLimiter,
        FIELDS(
            MODULE_TYPE(Chronometry, Duration) target;
            MODULE_TYPE(Chronometry, ClockTimePoint) frameStart;
        )
    ),

    PACKAGE(Timer,
        OPERATION(Start, (MODULE_TYPE(Chronometry, Timer) *timer)),
        OPERATION(Elapsed,
                  (const MODULE_TYPE(Chronometry, Timer) *timer,
                   MODULE_TYPE(Chronometry, Duration) *outElapsed)),
        OPERATION(Restart,
                  (MODULE_TYPE(Chronometry, Timer) *timer,
                   MODULE_TYPE(Chronometry, Duration) *outElapsed)),

        SPACE(Delta,
              OPERATION(Start, (MODULE_TYPE(Chronometry, Delta) *delta)),
              OPERATION(Update,
                        (MODULE_TYPE(Chronometry, Delta) *delta,
                         MODULE_TYPE(Chronometry, Duration) *outElapsed))
        ),

        SPACE(FrameLimiter,
              OPERATION(Create,
                        (MODULE_TYPE(Chronometry, Duration) target,
                         MODULE_TYPE(Chronometry, FrameLimiter) *outLimiter)),
              OPERATION(FromFPS,
                        (uint32_t fps,
                         MODULE_TYPE(Chronometry, FrameLimiter) *outLimiter)),
              OPERATION(Begin,
                        (MODULE_TYPE(Chronometry, FrameLimiter) *limiter)),
              OPERATION(Remaining,
                        (const MODULE_TYPE(Chronometry, FrameLimiter) *limiter,
                         MODULE_TYPE(Chronometry, Duration) *outRemaining))
        )
    )
);

#include "Impl/Timer.impl"

/* EOF */
