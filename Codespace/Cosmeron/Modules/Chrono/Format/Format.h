#pragma once

#include "../Timezone/Timezone.h"
#include <stddef.h>

/* ========================================================================== */
/* 1. Decoupled X-Macro Buffer Capacity Table (Row-Tuple Pattern)             */
/* ========================================================================== */

#define CHRONO_FORMAT_CAPACITY_COL_NAME(NAME, VALUE) NAME
#define CHRONO_FORMAT_CAPACITY_COL_VALUE(NAME, VALUE) VALUE
#define CHRONO_FORMAT_CAPACITY_GET(FIELD, ROW)                                 \
  PP_OP_CAT2(CHRONO_FORMAT_CAPACITY_COL_, FIELD) ROW

#define CHRONO_FORMAT_CAPACITY_TABLE(X)                                        \
  X((ISO_DATE_BUFFER_SIZE, 16))                                                \
  X((ISO_TIME_BUFFER_SIZE, 16))                                                \
  X((ISO_WEEK_DATE_BUFFER_SIZE, 16))                                           \
  X((ISO_CIVIL_BUFFER_SIZE, 32))                                               \
  X((ISO_CIVIL_OFFSET_BUFFER_SIZE, 40))                                        \
  X((ISO_INSTANT_BUFFER_SIZE, 32))                                             \
  X((ISO_SPAN_BUFFER_SIZE, 48))                                                \
  X((RFC3339_BUFFER_SIZE, 40))                                                 \
  X((RFC2822_BUFFER_SIZE, 40))                                                 \
  X((HUMAN_SPAN_BUFFER_SIZE, 64))

/* ========================================================================== */
/* 2. Declarative Format Package                                              */
/* ========================================================================== */

MODULE(DOMAIN(Modules), Chrono,
    PACKAGE(Format,
        ENUM(BufferCapacity, CHRONO_FORMAT_CAPACITY_TABLE),

        VARIANT(ToISO,
                OPERATION(Date,
                          (MODULE_TYPE(Chrono, Date) date, char *buffer,
                           size_t capacity)),
                OPERATION(TimeOfDay,
                          (MODULE_TYPE(Chrono, TimeOfDay) time, char *buffer,
                           size_t capacity)),
                OPERATION(DateIsoWeek,
                          (MODULE_TYPE(Chrono, DateIsoWeek) isoWeek,
                           char *buffer, size_t capacity)),
                OPERATION(CivilStamp,
                          (MODULE_TYPE(Chrono, CivilStamp) value, char *buffer,
                           size_t capacity)),
                OPERATION(CivilOffset,
                          (MODULE_TYPE(Chrono, CivilOffset) value, char *buffer,
                           size_t capacity)),
                OPERATION(Instant,
                          (MODULE_TYPE(Chrono, Instant) value, char *buffer,
                           size_t capacity)),
                OPERATION(Span,
                          (MODULE_TYPE(Chrono, Span) value, char *buffer,
                           size_t capacity))
        ),

        VARIANT(FromISO,
                OPERATION(Date,
                          (const char *text, MODULE_TYPE(Chrono, Date) *date)),
                OPERATION(TimeOfDay,
                          (const char *text,
                           MODULE_TYPE(Chrono, TimeOfDay) *outTime)),
                OPERATION(DateIsoWeek,
                          (const char *text,
                           MODULE_TYPE(Chrono, DateIsoWeek) *outIsoWeek)),
                OPERATION(CivilStamp,
                          (const char *text,
                           MODULE_TYPE(Chrono, CivilStamp) *value)),
                OPERATION(CivilOffset,
                          (const char *text,
                           MODULE_TYPE(Chrono, CivilOffset) *value)),
                OPERATION(Instant,
                          (const char *text,
                           MODULE_TYPE(Chrono, Instant) *value)),
                OPERATION(Span,
                          (const char *text, MODULE_TYPE(Chrono, Span) *value))
        ),

        OPERATION(ToRFC3339,
                  (MODULE_TYPE(Chrono, CivilOffset) value, char *buffer,
                   size_t capacity)),
        OPERATION(FromRFC3339,
                  (const char *text,
                   MODULE_TYPE(Chrono, CivilOffset) *outValue)),
        OPERATION(ToRFC2822,
                  (MODULE_TYPE(Chrono, CivilOffset) value, char *buffer,
                   size_t capacity)),
        OPERATION(Pattern,
                  (char *buffer, size_t capacity, size_t *outWritten,
                   const char *pattern,
                   MODULE_TYPE(Chrono, CivilOffset) value)),
        OPERATION(ToHumanSpan,
                  (MODULE_TYPE(Chrono, Span) value, char *buffer,
                   size_t capacity))
    )
);

#include "Impl/Format.impl"

/* EOF */
