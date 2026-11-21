#pragma once

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Memory/Alloc.h"
#include "../../../Core/Module/Module.h"
#include "../../Tuple/Dual.h"
#include "../../Tuple/Quad.h"
#include "../Impl/Common.inc"

#define TEXT_ATTRIBUTE_FLAGS_COL_NAME(NAME, VALUE) NAME
#define TEXT_ATTRIBUTE_FLAGS_COL_VALUE(NAME, VALUE) VALUE
#define TEXT_ATTRIBUTE_FLAGS_GET(FIELD, ROW)                                   \
  PP_OP_CAT2(TEXT_ATTRIBUTE_FLAGS_COL_, FIELD) ROW

#define TEXT_ATTRIBUTE_FLAGS_TABLE(X)                                          \
  X((BOLD, UINT16_C(1) << 0U))                                                 \
  X((FAINT, UINT16_C(1) << 1U))                                                \
  X((ITALIC, UINT16_C(1) << 2U))                                               \
  X((BLINK, UINT16_C(1) << 3U))                                                \
  X((INVERSE, UINT16_C(1) << 4U))                                              \
  X((HIDDEN, UINT16_C(1) << 5U))                                               \
  X((STRIKE, UINT16_C(1) << 6U))                                               \
  X((OVERLINE, UINT16_C(1) << 7U))                                             \
  X((TRANSPARENT_BACKGROUND, UINT16_C(1) << 8U))                               \
  X((CODE_UNIT_CONTINUATION, UINT16_C(1) << 9U))                               \
  X((DEFAULT_FOREGROUND, UINT16_C(1) << 10U))                                  \
  X((DEFAULT_BACKGROUND, UINT16_C(1) << 11U))                                  \
  X((RESERVED, UINT16_C(1) << 12U))

ENUM_DEF(MODULE_NS(Text, Attribute), Flag, TEXT_ATTRIBUTE_FLAGS_TABLE)

#define TEXT_ATTRIBUTE_UNDERLINE_COL_NAME(NAME, VALUE) NAME
#define TEXT_ATTRIBUTE_UNDERLINE_COL_VALUE(NAME, VALUE) VALUE
#define TEXT_ATTRIBUTE_UNDERLINE_GET(FIELD, ROW)                               \
  PP_OP_CAT2(TEXT_ATTRIBUTE_UNDERLINE_COL_, FIELD) ROW

#define TEXT_ATTRIBUTE_UNDERLINE_TABLE(X)                                      \
  X((NONE, 0))                                                                 \
  X((SINGLE, 1))                                                               \
  X((DOUBLE, 2))                                                               \
  X((CURLY, 3))                                                                \
  X((DOTTED, 4))                                                               \
  X((DASHED, 5))                                                               \
  X((RESERVED_1, 6))                                                           \
  X((RESERVED_2, 7))

ENUM_DEF_EX(MODULE_NS(Text, Attribute), Underline,
            TEXT_ATTRIBUTE_UNDERLINE_TABLE, QUALIFIED)

#define TEXT_ATTRIBUTE_COLOR_MASK_COL_NAME(NAME, VALUE) NAME
#define TEXT_ATTRIBUTE_COLOR_MASK_COL_VALUE(NAME, VALUE) VALUE
#define TEXT_ATTRIBUTE_COLOR_MASK_GET(FIELD, ROW)                              \
  PP_OP_CAT2(TEXT_ATTRIBUTE_COLOR_MASK_COL_, FIELD) ROW

#define TEXT_ATTRIBUTE_COLOR_MASK_TABLE(X)                                     \
  X((RGB_MASK, UINT32_C(0x00FFFFFF)))                                          \
  X((CHANNEL_MASK, UINT32_C(0xFF)))                                            \
  X((GREEN_SHIFT, 8U))                                                         \
  X((RED_SHIFT, 16U))

IENUM_DEF(MODULE_NS(Text, Attribute, Color), Mask,
          TEXT_ATTRIBUTE_COLOR_MASK_TABLE)

#define TEXT_ATTRIBUTE_WINDOWS_COLOR_COL_NAME(NAME, VALUE) NAME
#define TEXT_ATTRIBUTE_WINDOWS_COLOR_COL_VALUE(NAME, VALUE) VALUE
#define TEXT_ATTRIBUTE_WINDOWS_COLOR_GET(FIELD, ROW)                           \
  PP_OP_CAT2(TEXT_ATTRIBUTE_WINDOWS_COLOR_COL_, FIELD) ROW

#define TEXT_ATTRIBUTE_WINDOWS_COLOR_TABLE(X)                                  \
  X((BLACK, UINT32_C(0x0C0C0C)))                                               \
  X((DARK_BLUE, UINT32_C(0x0037DA)))                                           \
  X((DARK_GREEN, UINT32_C(0x13A10E)))                                          \
  X((DARK_CYAN, UINT32_C(0x3A96DD)))                                           \
  X((DARK_RED, UINT32_C(0xC50F1F)))                                            \
  X((DARK_MAGENTA, UINT32_C(0x881798)))                                        \
  X((DARK_YELLOW, UINT32_C(0xC19C00)))                                         \
  X((LIGHT_GRAY, UINT32_C(0xCCCCCC)))                                          \
  X((DARK_GRAY, UINT32_C(0x767676)))                                           \
  X((BRIGHT_BLUE, UINT32_C(0x3B78FF)))                                         \
  X((BRIGHT_GREEN, UINT32_C(0x16C60C)))                                        \
  X((BRIGHT_CYAN, UINT32_C(0x61D6D6)))                                         \
  X((BRIGHT_RED, UINT32_C(0xE74856)))                                          \
  X((BRIGHT_MAGENTA, UINT32_C(0xB4009E)))                                      \
  X((BRIGHT_YELLOW, UINT32_C(0xF9F1A5)))                                       \
  X((WHITE, UINT32_C(0xF2F2F2)))

#define TEXT_ATTRIBUTE_WINDOWS_COUNT_COL_NAME(NAME, VALUE) NAME
#define TEXT_ATTRIBUTE_WINDOWS_COUNT_COL_VALUE(NAME, VALUE) VALUE
#define TEXT_ATTRIBUTE_WINDOWS_COUNT_GET(FIELD, ROW)                           \
  PP_OP_CAT2(TEXT_ATTRIBUTE_WINDOWS_COUNT_COL_, FIELD) ROW

#define TEXT_ATTRIBUTE_WINDOWS_COUNT_TABLE(X) X((WINDOWS_COUNT, 16U))

ENUM_DEF_EX(MODULE_NS(Text, Attribute_Color), Windows,
            TEXT_ATTRIBUTE_WINDOWS_COLOR_TABLE, QUALIFIED)
ENUM_DEF_EX(MODULE_NS(Text, Attribute_Color), WindowsIndex,
            TEXT_ATTRIBUTE_WINDOWS_COLOR_TABLE, QUALIFIED_NAME)
ENUM_DEF(MODULE_NS(Text, Attribute_Color), WindowsCount,
         TEXT_ATTRIBUTE_WINDOWS_COUNT_TABLE)

MODULE(DOMAIN(Modules), Text,
    STRUCT(Attribute_Cell,
        FIELDS(
            uint32_t foreground : 24;
            uint32_t background : 24;
            uint16_t flags : 13;
            uint8_t underline : 3;
        )
    ),
    STRUCT(Attribute_Grid,
        FIELDS(
            MODULE_TYPE(Text, Attribute, Cell) *data;
            MODULE_TYPED(Tuple, Dual, UInt16) size;
        )
    ),
    PACKAGE(Attribute,
        FUNCTION(MODULE_TYPE(Text, Attribute, Cell), Default, ()),
        PREDICATE(IsEmpty, (const MODULE_TYPE(Text, Attribute, Grid) *grid)),
        PREDICATE(IsValid, (const MODULE_TYPE(Text, Attribute, Grid) *grid)),
        OPERATION(Create, (MODULE_TYPE(Text, Attribute, Grid) *grid,
                           MODULE_TYPED(Tuple, Dual, UInt16) size)),
        OPERATION(Destroy, (MODULE_TYPE(Text, Attribute, Grid) *grid)),
        OPERATION(Resize, (MODULE_TYPE(Text, Attribute, Grid) *grid,
                           MODULE_TYPED(Tuple, Dual, UInt16) size)),
        OPERATION(Recreate, (MODULE_TYPE(Text, Attribute, Grid) *grid,
                             MODULE_TYPED(Tuple, Dual, UInt16) size)),
        OPERATION(Clone, (MODULE_TYPE(Text, Attribute, Grid) *destination,
                          const MODULE_TYPE(Text, Attribute, Grid) *source)),
        VARIANT(Clear,
                OPERATION((MODULE_TYPE(Text, Attribute, Grid) *grid)),
                OPERATION(With, (MODULE_TYPE(Text, Attribute, Grid) *grid,
                                 MODULE_TYPE(Text, Attribute, Cell) attribute))
        ),
        OPERATION(Fill, (MODULE_TYPE(Text, Attribute, Grid) *grid,
                         MODULE_TYPE(Text, Attribute, Cell) attribute)),
        OPERATION(ReadCell, (const MODULE_TYPE(Text, Attribute, Grid) *grid,
                             MODULE_TYPED(Tuple, Dual, UInt16) position,
                             MODULE_TYPE(Text, Attribute, Cell) *attribute)),
        OPERATION(WriteCell, (MODULE_TYPE(Text, Attribute, Grid) *grid,
                              MODULE_TYPED(Tuple, Dual, UInt16) position,
                              MODULE_TYPE(Text, Attribute, Cell) attribute)),
        OPERATION(FillRegion, (MODULE_TYPE(Text, Attribute, Grid) *grid,
                               MODULE_TYPED(Tuple, Quad, UInt16) region,
                               MODULE_TYPE(Text, Attribute, Cell) attribute)),
        OPERATION(Read, (const MODULE_TYPE(Text, Attribute, Grid) *source,
                         MODULE_TYPED(Tuple, Quad, UInt16) region,
                         MODULE_TYPE(Text, Attribute, Grid) *destination)),
        VARIANT(Write,
                OPERATION((MODULE_TYPE(Text, Attribute, Grid) *destination,
                           MODULE_TYPED(Tuple, Dual, UInt16) destinationPosition,
                           const MODULE_TYPE(Text, Attribute, Grid) *source)),
                OPERATION(Region,
                          (MODULE_TYPE(Text, Attribute, Grid) *destination,
                           MODULE_TYPED(Tuple, Dual, UInt16) destinationPosition,
                           const MODULE_TYPE(Text, Attribute, Grid) *source,
                           MODULE_TYPED(Tuple, Quad, UInt16) sourceRegion))
        ),
        SPACE(Color,
            FUNCTION(uint32_t, RGB,
                     (uint8_t red, uint8_t green, uint8_t blue)),
            FUNCTION(uint8_t, Red, (uint32_t color)),
            FUNCTION(uint8_t, Green, (uint32_t color)),
            FUNCTION(uint8_t, Blue, (uint32_t color)),
            FUNCTION(const uint32_t *, WindowsPalette, ())
        )
    )
);

#include "Impl/Attribute.impl"

/* EOF */
