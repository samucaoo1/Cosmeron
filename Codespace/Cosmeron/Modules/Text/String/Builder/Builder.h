#pragma once

#include "../../../../Core/Condition/Status.h"
#include "../../../../Core/Memory/Alloc.h"
#include "../../../../Core/Module/Object.h"
#include "../../Encoding/UTF8.h"
#include "../../Format/Format.h"
#include "../../View/View.h"
#include "../String.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

enum {
  CNS(Text, Builder, INTERNAL, INITIAL_CAPACITY) = 32U,
  CNS(Text, Builder, INTERNAL, GROWTH_FACTOR) = 2U
};

/* Object-oriented StringBuilder definition under Text_Builder namespace */
MODULE(Text,
    OBJECT(Builder,
           FIELDS(char *buffer; size_t size; size_t capacity;),
       METHOD(OPSTATUS, Create, (size_t initialCapacity)),
       METHOD(OPSTATUS, Destroy, ()), METHOD(OPSTATUS, Clear, ()),
       METHOD(OPSTATUS, Reserve, (size_t newCapacity)),
       METHOD(OPSTATUS, ShrinkToFit, ()),
       METHOD(OPSTATUS, AppendChar, (char ch)),
       METHOD(OPSTATUS, AppendCStr, (const char *cstr)),
       METHOD(OPSTATUS, AppendBuffer, (const char *buffer, size_t length)),
       METHOD(OPSTATUS, AppendView, (MODULE_TYPE(Text, View, 8) view)),
       METHOD(OPSTATUS, AppendCodepoint, (uint32_t codepoint)),
       METHOD(OPSTATUS, AppendFormatVariadic,
              (const char *format, va_list args)),
       METHOD(OPSTATUS, BuildString, (MODULE_TYPE(Text, String, 8) * outString)),
       CONST_METHOD(size_t, Length, ()), CONST_METHOD(size_t, Capacity, ()),
       CONST_METHOD(bool, IsEmpty, ()), CONST_METHOD(const char *, CStr, ()),
       CONST_METHOD(MODULE_TYPE(Text, View, 8), View, ()))
);

#define TEXT_BUILDER_APPEND_FORMAT_PROTOTYPE                                   \
  static inline OPSTATUS MODULE_FUNC(Text, Builder, AppendFormat)(                      \
      MODULE_TYPE(Text, Builder) * self, const char *format, ...)

#include "Impl/Builder.impl"

/* EOF */
