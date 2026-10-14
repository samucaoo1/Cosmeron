#pragma once

#include "../../../Core/Error/Status.h"
#include "../../Struct/TDual.h"
#include "../../Struct/TPair.h"
#include "../../Struct/TPenta.h"
#include "../../Struct/TQuad.h"
#include "../../Struct/TTriple.h"
#include "../../Type/TBigint.h"
#include "../../Type/TDecimal.h"
#include "../Encoding/UTF8.h"
#include "../Text.space"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define TEXT_FORMAT_PROTOTYPE                                                  \
  static inline OPSTATUS TEXT_FUNC(Format)(                                    \
      char *buffer, size_t capacity, size_t *outWritten, const char *format,   \
      ...)

#define TEXT_FORMAT_VARIADIC_PROTOTYPE                                         \
  static inline OPSTATUS TEXT_FORMAT_FUNC(Variadic)(                           \
      char *buffer, size_t capacity, size_t *outWritten, const char *format,   \
      va_list arguments)

#define TEXT_FORMAT_UNSAFE_PROTOTYPE                                           \
  static inline OPSTATUS TEXT_FORMAT_FUNC(Unsafe)(                             \
      char *buffer, size_t *outWritten, const char *format, ...)

#define TEXT_FORMAT_UNSAFE_VARIADIC_PROTOTYPE                                  \
  static inline OPSTATUS TEXT_FORMAT_FUNC(Unsafe_Variadic)(                    \
      char *buffer, size_t *outWritten, const char *format, va_list arguments)

TEXT_FORMAT_PROTOTYPE;
TEXT_FORMAT_VARIADIC_PROTOTYPE;
TEXT_FORMAT_UNSAFE_PROTOTYPE;
TEXT_FORMAT_UNSAFE_VARIADIC_PROTOTYPE;

#include "Impl/Format.impl"
/* EOF */
