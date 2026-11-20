#pragma once

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Module/Module.h"
#include "../../../Core/Types/TaggedUnion.h"
#include "../../Tuple/Dual.h"
#include "../../Tuple/Pair.h"
#include "../../Tuple/Penta.h"
#include "../../Tuple/Quad.h"
#include "../../Tuple/Triple.h"
#include "../../Numeric/Bigint.h"
#include "../../Numeric/Decimal.h"
#include "../Encoding/UTF8.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(TEXT_DISABLE_FUNCTION_TABLE) && !defined(COSMERON_DISABLE_FT_Text)
#define COSMERON_DISABLE_FT_Text 1
#endif
#if defined(TEXT_FORMAT_DISABLE_FUNCTION_TABLE) &&                             \
    !defined(COSMERON_DISABLE_FT_Text_Format)
#define COSMERON_DISABLE_FT_Text_Format 1
#endif

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

/* EOF */
