#pragma once

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Module/Module.h"
#include "../../../Core/Types/TaggedUnion.h"
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
        VARIANT(Variadic,
                OPERATION((char *buffer, size_t capacity, size_t *outWritten,
                           const char *format, va_list arguments)),
                OPERATION(Unsafe,
                          (char *buffer, size_t *outWritten, const char *format,
                           va_list arguments))
        ),
        OPERATION_INLINE(Buffer,
                         (char *buffer, size_t capacity, size_t *outWritten,
                          const char *format, ...), {
          OPSTATUS status;
          va_list args;
          va_start(args, format);
          status = MODULE_FUNC(Text, Format, Variadic)(buffer, capacity,
                                                       outWritten, format, args);
          va_end(args);
          return status;
        }),
        OPERATION_INLINE(Unsafe,
                         (char *buffer, size_t *outWritten, const char *format, ...), {
          OPSTATUS status;
          va_list args;
          va_start(args, format);
          status = MODULE_FUNC(Text, Format, VariadicUnsafe)(buffer, outWritten, format, args);
          va_end(args);
          return status;
        })
    )
);

#include "Impl/Format.impl"
#include "BytesHuman.h"
