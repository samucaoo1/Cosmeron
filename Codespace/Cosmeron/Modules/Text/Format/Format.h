#pragma once

#include "../../../Core/Condition/Status.h"
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

#define TEXT_FORMAT_PROTOTYPE                                                  \
  static inline OPSTATUS MODULE_FUNC(Text, Format)(                                    \
      char *buffer, size_t capacity, size_t *outWritten, const char *format,   \
      ...)

#define TEXT_FORMAT_VARIADIC_PROTOTYPE                                         \
  static inline OPSTATUS MODULE_FUNC(Text, Format, Variadic)(                           \
      char *buffer, size_t capacity, size_t *outWritten, const char *format,   \
      va_list arguments)

#define TEXT_FORMAT_UNSAFE_PROTOTYPE                                           \
  static inline OPSTATUS MODULE_FUNC(Text, Format, Unsafe)(                             \
      char *buffer, size_t *outWritten, const char *format, ...)

#define TEXT_FORMAT_UNSAFE_VARIADIC_PROTOTYPE                                  \
  static inline OPSTATUS MODULE_FUNC(Text, Format, Unsafe_Variadic)(                    \
      char *buffer, size_t *outWritten, const char *format, va_list arguments)

TEXT_FORMAT_PROTOTYPE;
TEXT_FORMAT_VARIADIC_PROTOTYPE;
TEXT_FORMAT_UNSAFE_PROTOTYPE;
TEXT_FORMAT_UNSAFE_VARIADIC_PROTOTYPE;

#include "Impl/Format.impl"
#include "BytesHuman.h"
/* EOF */
