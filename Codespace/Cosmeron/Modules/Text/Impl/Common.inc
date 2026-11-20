#pragma once

#include <stdint.h>

#include "../../../Core/Algorithm/Comparison.h"
#include "../../../Core/Condition/Status.h"
#include "../../../Core/Module/Module.h"

#if defined(TEXT_DISABLE_FUNCTION_TABLE) && !defined(COSMERON_DISABLE_FT_Text)
#define COSMERON_DISABLE_FT_Text 1
#endif
#if defined(TEXT_VIEW_DISABLE_FUNCTION_TABLE) &&                               \
    !defined(COSMERON_DISABLE_FT_Text_View)
#define COSMERON_DISABLE_FT_Text_View 1
#endif
#if defined(TEXT_VIEW_DISABLE_FUNCTION_TABLE) &&                               \
    !defined(COSMERON_DISABLE_FT_Text_View8)
#define COSMERON_DISABLE_FT_Text_View8 1
#endif
#if defined(TEXT_VIEW_DISABLE_FUNCTION_TABLE) &&                               \
    !defined(COSMERON_DISABLE_FT_Text_View16)
#define COSMERON_DISABLE_FT_Text_View16 1
#endif
#if defined(TEXT_VIEW_DISABLE_FUNCTION_TABLE) &&                               \
    !defined(COSMERON_DISABLE_FT_Text_View32)
#define COSMERON_DISABLE_FT_Text_View32 1
#endif
#if defined(TEXT_STRING_DISABLE_FUNCTION_TABLE) &&                             \
    !defined(COSMERON_DISABLE_FT_Text_String)
#define COSMERON_DISABLE_FT_Text_String 1
#endif
#if defined(TEXT_STRING_DISABLE_FUNCTION_TABLE) &&                             \
    !defined(COSMERON_DISABLE_FT_Text_String8)
#define COSMERON_DISABLE_FT_Text_String8 1
#endif
#if defined(TEXT_STRING_DISABLE_FUNCTION_TABLE) &&                             \
    !defined(COSMERON_DISABLE_FT_Text_String16)
#define COSMERON_DISABLE_FT_Text_String16 1
#endif
#if defined(TEXT_STRING_DISABLE_FUNCTION_TABLE) &&                             \
    !defined(COSMERON_DISABLE_FT_Text_String32)
#define COSMERON_DISABLE_FT_Text_String32 1
#endif

/* ========================================================================== */
/* Text Character Types                                                       */
/* ========================================================================== */

typedef uint8_t MODULE_TYPE(Text, Char8);
typedef uint16_t MODULE_TYPE(Text, Char16);
typedef uint32_t MODULE_TYPE(Text, Char32);

#define TEXT_GRID_CHARACTER_TABLE(X)                                           \
  X(MODULE_TYPE(Text, Char8), Char8)                                           \
  X(MODULE_TYPE(Text, Char16), Char16)                                         \
  X(MODULE_TYPE(Text, Char32), Char32)

/* ========================================================================== */
/* Text View & String Internal Configuration Tables                           */
/* ========================================================================== */

#define TEXT_VIEW_INTERNAL_COL_NAME(NAME, VALUE) NAME
#define TEXT_VIEW_INTERNAL_COL_VALUE(NAME, VALUE) VALUE
#define TEXT_VIEW_INTERNAL_GET(FIELD, ROW)                                     \
  PP_OP_CAT2(TEXT_VIEW_INTERNAL_COL_, FIELD) ROW

#define TEXT_VIEW_INTERNAL_TABLE(X)                                            \
  X((UTF8_CONT_MASK, 0xC0U))                                                   \
  X((UTF8_CONT_TAG, 0x80U))                                                    \
  X((UTF8_MAX_BYTES, 4U))

IENUM_DEF(Text_View, Config, TEXT_VIEW_INTERNAL_TABLE)

#define TEXT_STRING_INTERNAL_COL_NAME(NAME, VALUE) NAME
#define TEXT_STRING_INTERNAL_COL_VALUE(NAME, VALUE) VALUE
#define TEXT_STRING_INTERNAL_GET(FIELD, ROW)                                   \
  PP_OP_CAT2(TEXT_STRING_INTERNAL_COL_, FIELD) ROW

#define TEXT_STRING_INTERNAL_TABLE(X)                                          \
  X((INITIAL_CAPACITY, 16U))                                                   \
  X((GROWTH_FACTOR, 2U))                                                       \
  X((UTF8_MAX_BYTES, 4U))

IENUM_DEF(Text_String, Config, TEXT_STRING_INTERNAL_TABLE)

#ifndef TEMPLATE_Text
#define TEMPLATE_Text(TARGET, ...) NS(TEMPLATE_Text, TARGET)(__VA_ARGS__)
#endif

/* EOF */
