#pragma once

#include "../Impl/Common.inc"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* 1. Decoupled X-Macro Unit & Granularity Tables (Row-Tuple Pattern)         */
/* ========================================================================== */

#define CHRONO_MATH_UNIT_COL_NAME(NAME, VALUE) NAME
#define CHRONO_MATH_UNIT_COL_VALUE(NAME, VALUE) VALUE
#define CHRONO_MATH_UNIT_GET(FIELD, ROW)                                       \
  PP_OP_CAT2(CHRONO_MATH_UNIT_COL_, FIELD) ROW

#define CHRONO_MATH_UNIT_TABLE(X)                                              \
  X((NANOSECONDS_PER_MICROSECOND, 1000))                                       \
  X((NANOSECONDS_PER_MILLISECOND, 1000000))                                    \
  X((NANOSECONDS_PER_SECOND, 1000000000))                                      \
  X((MICROSECONDS_PER_MILLISECOND, 1000))                                      \
  X((MICROSECONDS_PER_SECOND, 1000000))                                        \
  X((MILLISECONDS_PER_SECOND, 1000))                                           \
  X((SECONDS_PER_MINUTE, 60))                                                  \
  X((MINUTES_PER_HOUR, 60))                                                    \
  X((HOURS_PER_DAY, 24))                                                       \
  X((MINUTES_PER_DAY, 1440))                                                   \
  X((SECONDS_PER_HOUR, 3600))                                                  \
  X((SECONDS_PER_DAY, 86400))

#define CHRONO_MATH_GRANULARITY_COL_NAME(NAME) NAME
#define CHRONO_MATH_GRANULARITY_GET(FIELD, ROW)                                \
  PP_OP_CAT2(CHRONO_MATH_GRANULARITY_COL_, FIELD) ROW

#define CHRONO_MATH_GRANULARITY_TABLE(X)                                       \
  X((GRANULARITY_NANOSECOND))                                                  \
  X((GRANULARITY_MICROSECOND))                                                 \
  X((GRANULARITY_MILLISECOND))                                                 \
  X((GRANULARITY_SECOND))                                                      \
  X((GRANULARITY_MINUTE))                                                      \
  X((GRANULARITY_HOUR))                                                        \
  X((GRANULARITY_DAY))

#define COSMERON_MACRO_INTERNAL_CHRONO_GRANULARITY_DISP(TARGET, ENUM_NAME,     \
                                                        ROW)                   \
  CNS(TARGET, CHRONO_MATH_GRANULARITY_GET(NAME, ROW)),

ENUM_DEF_EX(MODULE_NS(Chrono, Math), Granularity,
            CHRONO_MATH_GRANULARITY_TABLE,
            COSMERON_MACRO_INTERNAL_CHRONO_GRANULARITY_DISP)

#undef COSMERON_MACRO_INTERNAL_CHRONO_GRANULARITY_DISP

/* ========================================================================== */
/* 2. Declarative Structs & Math Package                                      */
/* ========================================================================== */

MODULE(DOMAIN(Modules), Chrono,
    STRUCT(Span,
        FIELDS(
            int64_t nanoseconds;
        )
    ),

    STRUCT(Interval,
        FIELDS(
            MODULE_TYPE(Chrono, Span) start;
            MODULE_TYPE(Chrono, Span) end;
        )
    ),

    PACKAGE(Math,
        ENUM(Unit, CHRONO_MATH_UNIT_TABLE),

        VARIANT(From,
                FUNCTION(MODULE_TYPE(Chrono, Span), Nanoseconds,
                         (int64_t nanoseconds)),
                OPERATION(Microseconds,
                          (int64_t microseconds,
                           MODULE_TYPE(Chrono, Span) *outResult)),
                OPERATION(Milliseconds,
                          (int64_t milliseconds,
                           MODULE_TYPE(Chrono, Span) *outResult)),
                OPERATION(Seconds,
                          (int64_t seconds,
                           MODULE_TYPE(Chrono, Span) *outResult)),
                OPERATION(Minutes,
                          (int64_t minutes,
                           MODULE_TYPE(Chrono, Span) *outResult)),
                OPERATION(Hours,
                          (int64_t hours,
                           MODULE_TYPE(Chrono, Span) *outResult)),
                OPERATION(Days,
                          (int64_t days,
                           MODULE_TYPE(Chrono, Span) *outResult)),
                OPERATION(Hz,
                          (double hz,
                           MODULE_TYPE(Chrono, Span) *outResult))
        ),

        VARIANT(To,
                FUNCTION(int64_t, Nanoseconds,
                         (MODULE_TYPE(Chrono, Span) value)),
                FUNCTION(int64_t, Microseconds,
                         (MODULE_TYPE(Chrono, Span) value)),
                FUNCTION(int64_t, Milliseconds,
                         (MODULE_TYPE(Chrono, Span) value)),
                FUNCTION(int64_t, Seconds,
                         (MODULE_TYPE(Chrono, Span) value)),
                OPERATION(Hz,
                          (MODULE_TYPE(Chrono, Span) value, double *outHz))
        ),

        PREDICATE(IsZero, (MODULE_TYPE(Chrono, Span) value)),
        PREDICATE(IsNegative, (MODULE_TYPE(Chrono, Span) value)),
        COMPARISON(Compare,
                   (MODULE_TYPE(Chrono, Span) left,
                    MODULE_TYPE(Chrono, Span) right)),
        OPERATION(Add,
                  (MODULE_TYPE(Chrono, Span) left,
                   MODULE_TYPE(Chrono, Span) right,
                   MODULE_TYPE(Chrono, Span) *outResult)),
        OPERATION(Subtract,
                  (MODULE_TYPE(Chrono, Span) left,
                   MODULE_TYPE(Chrono, Span) right,
                   MODULE_TYPE(Chrono, Span) *outResult)),
        OPERATION(Absolute,
                  (MODULE_TYPE(Chrono, Span) value,
                   MODULE_TYPE(Chrono, Span) *outResult)),
        OPERATION(Negate,
                  (MODULE_TYPE(Chrono, Span) value,
                   MODULE_TYPE(Chrono, Span) *outResult)),
        OPERATION(Scale,
                  (MODULE_TYPE(Chrono, Span) value, int64_t factor,
                   MODULE_TYPE(Chrono, Span) *outResult)),
        OPERATION(Divide,
                  (MODULE_TYPE(Chrono, Span) value, int64_t divisor,
                   MODULE_TYPE(Chrono, Span) *outResult)),
        OPERATION(Remainder,
                  (MODULE_TYPE(Chrono, Span) value,
                   MODULE_TYPE(Chrono, Span) divisor,
                   MODULE_TYPE(Chrono, Span) *outResult)),
        OPERATION(Ratio,
                  (MODULE_TYPE(Chrono, Span) numerator,
                   MODULE_TYPE(Chrono, Span) denominator, double *outRatio)),
        FUNCTION(MODULE_TYPE(Chrono, Span), AddSaturating,
                 (MODULE_TYPE(Chrono, Span) left,
                  MODULE_TYPE(Chrono, Span) right)),
        FUNCTION(MODULE_TYPE(Chrono, Span), SubtractSaturating,
                 (MODULE_TYPE(Chrono, Span) left,
                  MODULE_TYPE(Chrono, Span) right)),
        FUNCTION(MODULE_TYPE(Chrono, Span), Min,
                 (MODULE_TYPE(Chrono, Span) left,
                  MODULE_TYPE(Chrono, Span) right)),
        FUNCTION(MODULE_TYPE(Chrono, Span), Max,
                 (MODULE_TYPE(Chrono, Span) left,
                  MODULE_TYPE(Chrono, Span) right)),
        OPERATION(Clamp,
                  (MODULE_TYPE(Chrono, Span) value,
                   MODULE_TYPE(Chrono, Span) minValue,
                   MODULE_TYPE(Chrono, Span) maxValue,
                   MODULE_TYPE(Chrono, Span) *outResult)),
        OPERATION(Truncate,
                  (MODULE_TYPE(Chrono, Span) value,
                   MODULE_ENUM(Chrono, Math, Granularity) unit,
                   MODULE_TYPE(Chrono, Span) *outResult)),
        OPERATION(Round,
                  (MODULE_TYPE(Chrono, Span) value,
                   MODULE_ENUM(Chrono, Math, Granularity) unit,
                   MODULE_TYPE(Chrono, Span) *outResult)),

        SPACE(Interval,
              PREDICATE(IsValid, (MODULE_TYPE(Chrono, Interval) interval)),
              OPERATION(Length,
                        (MODULE_TYPE(Chrono, Interval) interval,
                         MODULE_TYPE(Chrono, Span) *outSpan)),
              PREDICATE(Contains,
                        (MODULE_TYPE(Chrono, Interval) interval,
                         MODULE_TYPE(Chrono, Span) point)),
              PREDICATE(Overlaps,
                        (MODULE_TYPE(Chrono, Interval) left,
                         MODULE_TYPE(Chrono, Interval) right)),
              OPERATION(Intersection,
                        (MODULE_TYPE(Chrono, Interval) left,
                         MODULE_TYPE(Chrono, Interval) right,
                         MODULE_TYPE(Chrono, Interval) *outResult)),
              OPERATION(Union,
                        (MODULE_TYPE(Chrono, Interval) left,
                         MODULE_TYPE(Chrono, Interval) right,
                         MODULE_TYPE(Chrono, Interval) *outResult))
        )
    )
);

#include "Impl/Math.impl"

/* EOF */
