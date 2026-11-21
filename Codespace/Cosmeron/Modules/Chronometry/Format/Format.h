#pragma once

#include "../Timezone/Timezone.h"
#include <stddef.h>

/* ========================================================================== */
/* 1. Decoupled X-Macro Buffer Capacity Table (Row-Tuple Pattern)             */
/* ========================================================================== */

#define CHRONOMETRY_FORMAT_CAPACITY_COL_NAME(NAME, VALUE) NAME
#define CHRONOMETRY_FORMAT_CAPACITY_COL_VALUE(NAME, VALUE) VALUE
#define CHRONOMETRY_FORMAT_CAPACITY_GET(FIELD, ROW)                            \
  PP_OP_CAT2(CHRONOMETRY_FORMAT_CAPACITY_COL_, FIELD) ROW

#define CHRONOMETRY_FORMAT_CAPACITY_TABLE(X)                                   \
  X((ISO_DATE_BUFFER_SIZE, 16))                                                \
  X((ISO_TIMESTAMP_BUFFER_SIZE, 32))                                           \
  X((ISO_OFFSET_TIMESTAMP_BUFFER_SIZE, 40))                                    \
  X((ISO_INSTANT_BUFFER_SIZE, 32))                                             \
  X((ISO_DURATION_BUFFER_SIZE, 48))

/* ========================================================================== */
/* 2. Declarative Format Package                                              */
/* ========================================================================== */

MODULE(DOMAIN(Modules), Chronometry,
    PACKAGE(Format,
        ENUM(BufferCapacity, CHRONOMETRY_FORMAT_CAPACITY_TABLE),

        VARIANT(ToISO,
                OPERATION(Date,
                          (MODULE_TYPE(Chronometry, Date) date, char *buffer,
                           size_t capacity)),
                OPERATION(Timestamp,
                          (MODULE_TYPE(Chronometry, Timestamp) value,
                           char *buffer, size_t capacity)),
                OPERATION(OffsetTimestamp,
                          (MODULE_TYPE(Chronometry, OffsetTimestamp) value,
                           char *buffer, size_t capacity)),
                OPERATION(Instant,
                          (MODULE_TYPE(Chronometry, Instant) value,
                           char *buffer, size_t capacity)),
                OPERATION(Duration,
                          (MODULE_TYPE(Chronometry, Duration) value,
                           char *buffer, size_t capacity))
        ),

        VARIANT(FromISO,
                OPERATION(Date,
                          (const char *text,
                           MODULE_TYPE(Chronometry, Date) *date)),
                OPERATION(Timestamp,
                          (const char *text,
                           MODULE_TYPE(Chronometry, Timestamp) *value)),
                OPERATION(OffsetTimestamp,
                          (const char *text,
                           MODULE_TYPE(Chronometry, OffsetTimestamp) *value)),
                OPERATION(Instant,
                          (const char *text,
                           MODULE_TYPE(Chronometry, Instant) *value)),
                OPERATION(Duration,
                          (const char *text,
                           MODULE_TYPE(Chronometry, Duration) *value))
        )
    )
);

#include "Impl/Format.impl"

/* EOF */
