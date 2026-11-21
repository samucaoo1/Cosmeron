#pragma once

#include "../Impl/Common.inc"

#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* 1. Decoupled X-Macro Unit Table (Row-Tuple Pattern)                        */
/* ========================================================================== */

#define CHRONOMETRY_DURATION_UNIT_COL_NAME(NAME, VALUE) NAME
#define CHRONOMETRY_DURATION_UNIT_COL_VALUE(NAME, VALUE) VALUE
#define CHRONOMETRY_DURATION_UNIT_GET(FIELD, ROW)                              \
  PP_OP_CAT2(CHRONOMETRY_DURATION_UNIT_COL_, FIELD) ROW

#define CHRONOMETRY_DURATION_UNIT_TABLE(X)                                     \
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

/* ========================================================================== */
/* 2. Declarative Struct & Duration Package                                   */
/* ========================================================================== */

MODULE(DOMAIN(Modules), Chronometry,
    STRUCT(Duration,
        FIELDS(
            int64_t nanoseconds;
        )
    ),

    PACKAGE(Duration,
        ENUM(Unit, CHRONOMETRY_DURATION_UNIT_TABLE),

        VARIANT(From,
                FUNCTION(MODULE_TYPE(Chronometry, Duration), Nanoseconds,
                         (int64_t nanoseconds)),
                OPERATION(Microseconds,
                          (int64_t microseconds,
                           MODULE_TYPE(Chronometry, Duration) *outResult)),
                OPERATION(Milliseconds,
                          (int64_t milliseconds,
                           MODULE_TYPE(Chronometry, Duration) *outResult)),
                OPERATION(Seconds,
                          (int64_t seconds,
                           MODULE_TYPE(Chronometry, Duration) *outResult)),
                OPERATION(Minutes,
                          (int64_t minutes,
                           MODULE_TYPE(Chronometry, Duration) *outResult)),
                OPERATION(Hours,
                          (int64_t hours,
                           MODULE_TYPE(Chronometry, Duration) *outResult)),
                OPERATION(Days,
                          (int64_t days,
                           MODULE_TYPE(Chronometry, Duration) *outResult))
        ),

        VARIANT(To,
                FUNCTION(int64_t, Nanoseconds,
                         (MODULE_TYPE(Chronometry, Duration) value)),
                FUNCTION(int64_t, Microseconds,
                         (MODULE_TYPE(Chronometry, Duration) value)),
                FUNCTION(int64_t, Milliseconds,
                         (MODULE_TYPE(Chronometry, Duration) value)),
                FUNCTION(int64_t, Seconds,
                         (MODULE_TYPE(Chronometry, Duration) value))
        ),

        PREDICATE(IsZero, (MODULE_TYPE(Chronometry, Duration) value)),
        PREDICATE(IsNegative, (MODULE_TYPE(Chronometry, Duration) value)),
        OPERATION(Add,
                  (MODULE_TYPE(Chronometry, Duration) left,
                   MODULE_TYPE(Chronometry, Duration) right,
                   MODULE_TYPE(Chronometry, Duration) *outResult)),
        OPERATION(Subtract,
                  (MODULE_TYPE(Chronometry, Duration) left,
                   MODULE_TYPE(Chronometry, Duration) right,
                   MODULE_TYPE(Chronometry, Duration) *outResult)),
        COMPARISON(Compare,
                   (MODULE_TYPE(Chronometry, Duration) left,
                    MODULE_TYPE(Chronometry, Duration) right)),
        OPERATION(Absolute,
                  (MODULE_TYPE(Chronometry, Duration) value,
                   MODULE_TYPE(Chronometry, Duration) *outResult)),
        OPERATION(Negate,
                  (MODULE_TYPE(Chronometry, Duration) value,
                   MODULE_TYPE(Chronometry, Duration) *outResult)),
        OPERATION(Scale,
                  (MODULE_TYPE(Chronometry, Duration) value, int64_t factor,
                   MODULE_TYPE(Chronometry, Duration) *outResult))
    )
);

#include "Impl/Duration.impl"

/* EOF */
