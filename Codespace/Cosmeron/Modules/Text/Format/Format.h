#pragma once

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Module/Module.h"
#include "../Impl/Common.inc"
#include "../Unicode/UTF8.h"
#include "../View/View.h"
#include "Writer.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

MODULE(DOMAIN(Modules), Text,
    PACKAGE(Format,
        CALLABLE(
            OPERATION(Adapter,
                      (MODULE_TYPE(Text, Writer) *writer,
                       const void *value,
                       MODULE_TYPE(Text, View8) spec))
        ),

        VARIANT(To,
            OPERATION((MODULE_TYPE(Text, Writer) *writer,
                       const char *format, ...)),
            OPERATION(Variadic,
                      (MODULE_TYPE(Text, Writer) *writer,
                       const char *format, va_list arguments))
        ),

        VARIANT(Buffer,
            OPERATION((char *buffer, size_t capacity, size_t *outWritten,
                       const char *format, ...)),
            OPERATION(Variadic,
                      (char *buffer, size_t capacity, size_t *outWritten,
                       const char *format, va_list arguments))
        )
    )
);

#include "Impl/Format.impl"

/* EOF */
