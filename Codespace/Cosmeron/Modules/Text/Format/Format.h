#pragma once

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Module/Module.h"
#include "../../Tuple/TDual.h"
#include "../../Tuple/TPair.h"
#include "../../Tuple/TPenta.h"
#include "../../Tuple/TQuad.h"
#include "../../Tuple/TTriple.h"
#include "../../Numeric/TBigint.h"
#include "../../Numeric/TDecimal.h"
#include "../Encoding/UTF8.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

MODULE(DOMAIN(Modules), Text,
    PACKAGE(Format,
        OPERATION(Variadic,
                  (char *buffer, size_t capacity, size_t *outWritten,
                   const char *format, va_list arguments)),
        OPERATION(Unsafe_Variadic,
                  (char *buffer, size_t *outWritten, const char *format,
                   va_list arguments))
    )
);

static inline OPSTATUS MODULE_FUNC(Text, Format)(
    char *buffer, size_t capacity, size_t *outWritten, const char *format,
    ...);

static inline OPSTATUS MODULE_FUNC(Text, Format, Unsafe)(
    char *buffer, size_t *outWritten, const char *format, ...);

#include "Impl/Format.impl"
#include "BytesHuman.h"
