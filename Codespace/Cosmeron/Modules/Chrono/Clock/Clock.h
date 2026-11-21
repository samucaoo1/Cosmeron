#pragma once

#include "../Math/Math.h"
#include "../../../Core/Preprocessor/Detect/Detect.h"

/* ========================================================================== */
/* Declarative Structs & Clock Package                                        */
/* ========================================================================== */

MODULE(DOMAIN(Modules), Chrono,
    STRUCT(ClockPoint,
        FIELDS(
            uint64_t nanoseconds;
        )
    ),

    STRUCT(Instant,
        FIELDS(
            int64_t seconds;
            uint32_t nanoseconds;
        )
    ),

    STRUCT(Timer,
        FIELDS(
            MODULE_TYPE(Chrono, ClockPoint) start;
            MODULE_TYPE(Chrono, Span) accumulated;
            bool running;
        )
    ),

    STRUCT(TimerDelta,
        FIELDS(
            MODULE_TYPE(Chrono, ClockPoint) previous;
        )
    ),

    STRUCT(TimerLimiter,
        FIELDS(
            MODULE_TYPE(Chrono, Span) target;
            MODULE_TYPE(Chrono, ClockPoint) frameStart;
            bool started;
        )
    ),

    PACKAGE(Clock,
        OPERATION(Now, (MODULE_TYPE(Chrono, ClockPoint) *outResult)),
        OPERATION(SpanBetween,
                  (MODULE_TYPE(Chrono, ClockPoint) start,
                   MODULE_TYPE(Chrono, ClockPoint) end,
                   MODULE_TYPE(Chrono, Span) *outResult)),
        OPERATION(Sleep, (MODULE_TYPE(Chrono, Span) span)),
        OPERATION(SleepUntil, (MODULE_TYPE(Chrono, ClockPoint) target)),
        OPERATION(Resolution, (MODULE_TYPE(Chrono, Span) *outResult)),

        SPACE(Instant,
              OPERATION(Now, (MODULE_TYPE(Chrono, Instant) *outResult)),
              PREDICATE(IsValid, (MODULE_TYPE(Chrono, Instant) instant)),
              COMPARISON(Compare,
                         (MODULE_TYPE(Chrono, Instant) left,
                          MODULE_TYPE(Chrono, Instant) right)),
              OPERATION(AddSpan,
                        (MODULE_TYPE(Chrono, Instant) instant,
                         MODULE_TYPE(Chrono, Span) span,
                         MODULE_TYPE(Chrono, Instant) *outResult)),
              OPERATION(SpanBetween,
                        (MODULE_TYPE(Chrono, Instant) start,
                         MODULE_TYPE(Chrono, Instant) end,
                         MODULE_TYPE(Chrono, Span) *outResult)),
              FUNCTION(int64_t, ToUnixSeconds,
                       (MODULE_TYPE(Chrono, Instant) instant)),
              FUNCTION(MODULE_TYPE(Chrono, Instant), FromUnixSeconds,
                       (int64_t seconds)),
              OPERATION(ToUnixMillis,
                        (MODULE_TYPE(Chrono, Instant) instant,
                         int64_t *outMillis)),
              OPERATION(FromUnixMillis,
                        (int64_t millis,
                         MODULE_TYPE(Chrono, Instant) *outResult)),
              OPERATION(TaiMinusUtcSeconds,
                        (MODULE_TYPE(Chrono, Instant) utc,
                         int32_t *outSeconds)),
              OPERATION(ToTAI,
                        (MODULE_TYPE(Chrono, Instant) utc,
                         MODULE_TYPE(Chrono, Instant) *outTai)),
              OPERATION(FromTAI,
                        (MODULE_TYPE(Chrono, Instant) tai,
                         MODULE_TYPE(Chrono, Instant) *outUtc))
        ),

        SPACE(Timer,
              OPERATION(Start, (MODULE_TYPE(Chrono, Timer) *timer)),
              OPERATION(Pause, (MODULE_TYPE(Chrono, Timer) *timer)),
              OPERATION(Resume, (MODULE_TYPE(Chrono, Timer) *timer)),
              PREDICATE(IsRunning, (const MODULE_TYPE(Chrono, Timer) *timer)),
              OPERATION(Elapsed,
                        (const MODULE_TYPE(Chrono, Timer) *timer,
                         MODULE_TYPE(Chrono, Span) *outElapsed)),
              OPERATION(Restart,
                        (MODULE_TYPE(Chrono, Timer) *timer,
                         MODULE_TYPE(Chrono, Span) *outElapsed))
        ),

        SPACE(Delta,
              OPERATION(Start, (MODULE_TYPE(Chrono, TimerDelta) *delta)),
              OPERATION(Update,
                        (MODULE_TYPE(Chrono, TimerDelta) *delta,
                         MODULE_TYPE(Chrono, Span) *outElapsed))
        ),

        SPACE(Limiter,
              OPERATION(Create,
                        (MODULE_TYPE(Chrono, Span) target,
                         MODULE_TYPE(Chrono, TimerLimiter) *outLimiter)),
              OPERATION(FromHz,
                        (uint32_t hz,
                         MODULE_TYPE(Chrono, TimerLimiter) *outLimiter)),
              OPERATION(Begin, (MODULE_TYPE(Chrono, TimerLimiter) *limiter)),
              OPERATION(Remaining,
                        (const MODULE_TYPE(Chrono, TimerLimiter) *limiter,
                         MODULE_TYPE(Chrono, Span) *outRemaining)),
              OPERATION(Wait,
                        (const MODULE_TYPE(Chrono, TimerLimiter) *limiter))
        )
    )
);

#include "Impl/Clock.impl"

/* EOF */
