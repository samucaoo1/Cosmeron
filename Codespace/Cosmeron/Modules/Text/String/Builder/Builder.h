#pragma once

#include "../../../../Core/Error/Status.h"
#include "../../../../Core/Memory/Alloc.h"
#include "../../../../Core/Module/Object.h"
#include "../../Encoding/UTF8.h"
#include "../../Format/Format.h"
#include "../../Text.space"
#include "../../View/View.h"
#include "../String.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

enum {
  TEXT_BUILDER_CNS(INITIAL_CAPACITY) = 32U,
  TEXT_BUILDER_CNS(GROWTH_FACTOR) = 2U
};

/* Object-oriented StringBuilder definition under Text_Builder namespace */
OBJECT(TEXT_BUILDER_TYPE(TStringBuilder),
       FIELDS(char *buffer; size_t size; size_t capacity;),
       METHOD(OPSTATUS, Create, (size_t initialCapacity)),
       METHOD(OPSTATUS, Destroy, ()), METHOD(OPSTATUS, Clear, ()),
       METHOD(OPSTATUS, Reserve, (size_t newCapacity)),
       METHOD(OPSTATUS, ShrinkToFit, ()),
       METHOD(OPSTATUS, AppendChar, (char ch)),
       METHOD(OPSTATUS, AppendCStr, (const char *cstr)),
       METHOD(OPSTATUS, AppendBuffer, (const char *buffer, size_t length)),
       METHOD(OPSTATUS, AppendView, (TEXT_VIEW_TYPE(8) view)),
       METHOD(OPSTATUS, AppendCodepoint, (uint32_t codepoint)),
       METHOD(OPSTATUS, AppendFormatVariadic,
              (const char *format, va_list args)),
       METHOD(OPSTATUS, BuildString, (TEXT_STRING_TYPE(8) * outString)),
       CONST_METHOD(size_t, Length, ()), CONST_METHOD(size_t, Capacity, ()),
       CONST_METHOD(bool, IsEmpty, ()), CONST_METHOD(const char *, CStr, ()),
       CONST_METHOD(TEXT_VIEW_TYPE(8), View, ()));

/* User-facing canonical type */
typedef TEXT_BUILDER_TYPE(TStringBuilder) TStringBuilder;

#define TEXT_BUILDER_APPEND_FORMAT_PROTOTYPE                                   \
  static inline OPSTATUS TEXT_BUILDER_FUNC(AppendFormat)(                      \
      TEXT_BUILDER_TYPE(TStringBuilder) * self, const char *format, ...)

#include "Impl/Builder.impl"

/* EOF */
