#pragma once

#include "../../../../Core/Condition/Status.h"
#include "../../../../Core/Memory/Alloc.h"
#include "../../../../Core/Module/Module.h"
#include "../../Encoding/UTF8.h"
#include "../../Format/Format.h"
#include "../../View/View.h"
#include "../String.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#if defined(TEXT_DISABLE_FUNCTION_TABLE) && !defined(COSMERON_DISABLE_FT_Text)
#define COSMERON_DISABLE_FT_Text 1
#endif
#if defined(TEXT_BUILDER_DISABLE_FUNCTION_TABLE) &&                            \
    !defined(COSMERON_DISABLE_FT_Text_Builder)
#define COSMERON_DISABLE_FT_Text_Builder 1
#endif

#define TEXT_BUILDER_INTERNAL_COL_NAME(NAME, VALUE) NAME
#define TEXT_BUILDER_INTERNAL_COL_VALUE(NAME, VALUE) VALUE
#define TEXT_BUILDER_INTERNAL_GET(FIELD, ROW)                                  \
  PP_OP_CAT2(TEXT_BUILDER_INTERNAL_COL_, FIELD) ROW

#define TEXT_BUILDER_INTERNAL_TABLE(X)                                         \
  X((INITIAL_CAPACITY, 32U))                                                   \
  X((GROWTH_FACTOR, 2U))                                                       \
  X((UTF8_MAX_BYTES, 4U))

IENUM_DEF(Text_Builder, Config, TEXT_BUILDER_INTERNAL_TABLE)

/* Object-oriented StringBuilder definition under Text_Builder namespace */
MODULE(
    DOMAIN(Modules), Text,
    OBJECT(Builder, FIELDS(char *buffer; size_t size; size_t capacity;),
           OPERATION(Create, (size_t initialCapacity)), OPERATION(Destroy, ()),
           OPERATION(Clear, ()), OPERATION(Reserve, (size_t newCapacity)),
           OPERATION(ShrinkToFit, ()),
           VARIANT(Append, OPERATION(Char, (char ch)),
                   OPERATION(CStr, (const char *cstr)),
                   OPERATION(Buffer, (const char *buffer, size_t length)),
                   OPERATION(View, (MODULE_TYPE(Text, View8) view)),
                   OPERATION(Codepoint, (uint32_t codepoint)),
                   OPERATION(Format, (const char *format, ...)),
                   OPERATION(FormatVariadic,
                             (const char *format, va_list args))),
           OPERATION(BuildString, (MODULE_TYPE(Text, String8) * outString)),
           CONST_FUNCTION(size_t, Length, ()),
           CONST_FUNCTION(size_t, Capacity, ()), CONST_PREDICATE(IsEmpty, ()),
           CONST_FUNCTION(const char *, CStr, ()),
           CONST_FUNCTION(MODULE_TYPE(Text, View8), View, ())));

#include "Impl/Builder.impl"

/* EOF */
