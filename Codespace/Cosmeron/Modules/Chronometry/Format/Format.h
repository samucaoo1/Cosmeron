#pragma once

#include "../Timezone/Timezone.h"
#include <stddef.h>

/* ========================================================================== */
/* Declarative Format Package                                                 */
/* ========================================================================== */

MODULE(DOMAIN(Modules), Chronometry,
    PACKAGE(Format,
        SPACE(ISO,
              OPERATION(Date,
                        (MODULE_TYPE(Chronometry, Date) date, char *buffer,
                         size_t capacity)),
              OPERATION(DateTime,
                        (MODULE_TYPE(Chronometry, TimeStamp) value,
                         char *buffer, size_t capacity)),
              OPERATION(ParseDate,
                        (const char *text,
                         MODULE_TYPE(Chronometry, Date) *date)),
              OPERATION(ParseDateTime,
                        (const char *text,
                         MODULE_TYPE(Chronometry, TimeStamp) *value))
        )
    )
);

#include "Impl/Format.impl"

/* EOF */
