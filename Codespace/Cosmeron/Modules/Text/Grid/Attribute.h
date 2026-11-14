#pragma once

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Memory/Alloc.h"
#include "../../../Core/Module/Module.h"
#include "../../Tuple/TDual.h"
#include "../../Tuple/TQuad.h"

typedef uint16_t MODULE_TYPE(Text, Attribute, Flags);
typedef uint8_t MODULE_TYPE(Text, Grid_Underline, Style);
typedef uint32_t MODULE_TYPE(Text, Grid_Color, RGB);

enum {
  CNS(Text, Attribute, BOLD) = UINT16_C(1) << 0U,
  CNS(Text, Attribute, FAINT) = UINT16_C(1) << 1U,
  CNS(Text, Attribute, ITALIC) = UINT16_C(1) << 2U,
  CNS(Text, Attribute, BLINK) = UINT16_C(1) << 3U,
  CNS(Text, Attribute, INVERSE) = UINT16_C(1) << 4U,
  CNS(Text, Attribute, HIDDEN) = UINT16_C(1) << 5U,
  CNS(Text, Attribute, STRIKE) = UINT16_C(1) << 6U,
  CNS(Text, Attribute, OVERLINE) = UINT16_C(1) << 7U,
  CNS(Text, Attribute, TRANSPARENT_BACKGROUND) = UINT16_C(1) << 8U,
  CNS(Text, Attribute, CODE_UNIT_CONTINUATION) = UINT16_C(1) << 9U,
  CNS(Text, Attribute, DEFAULT_FOREGROUND) = UINT16_C(1) << 10U,
  CNS(Text, Attribute, DEFAULT_BACKGROUND) = UINT16_C(1) << 11U,
  CNS(Text, Attribute, RESERVED) = UINT16_C(1) << 12U
};

enum {
  CNS(Text, Grid_Underline, NONE) = 0,
  CNS(Text, Grid_Underline, SINGLE),
  CNS(Text, Grid_Underline, DOUBLE),
  CNS(Text, Grid_Underline, CURLY),
  CNS(Text, Grid_Underline, DOTTED),
  CNS(Text, Grid_Underline, DASHED),
  CNS(Text, Grid_Underline, RESERVED_1),
  CNS(Text, Grid_Underline, RESERVED_2)
};

typedef struct {
  uint32_t foreground : 24;
  uint32_t background : 24;
  uint16_t flags : 13;
  uint16_t underline : 3;
} MODULE_TYPE(Text, Attribute, Cell);

typedef struct {
  MODULE_TYPE(Text, Attribute, Cell) *data;
  MODULE_TYPED(Tuple, Dual, UInt16) size;
} MODULE_TYPE(Text, Attribute, Grid);

enum { CNS(Text, Grid_Color, RGB_MASK) = UINT32_C(0x00FFFFFF) };
#define TEXT_GRID_COLOR_RGB(RED, GREEN, BLUE)                                 \
  ((((uint32_t)(RED)&UINT32_C(0xFF)) << 16U) |                               \
   (((uint32_t)(GREEN)&UINT32_C(0xFF)) << 8U) |                              \
   ((uint32_t)(BLUE)&UINT32_C(0xFF)))
#define TEXT_GRID_COLOR_RED(COLOR)                                            \
  ((uint8_t)(((uint32_t)(COLOR) >> 16U) & UINT32_C(0xFF)))
#define TEXT_GRID_COLOR_GREEN(COLOR)                                          \
  ((uint8_t)(((uint32_t)(COLOR) >> 8U) & UINT32_C(0xFF)))
#define TEXT_GRID_COLOR_BLUE(COLOR)                                           \
  ((uint8_t)((uint32_t)(COLOR)&UINT32_C(0xFF)))

#define TEXT_GRID_WINDOWS_COLOR_TABLE                                         \
  X(BLACK, 0x0C0C0C) X(DARK_BLUE, 0x0037DA) X(DARK_GREEN, 0x13A10E)          \
  X(DARK_CYAN, 0x3A96DD) X(DARK_RED, 0xC50F1F)                               \
  X(DARK_MAGENTA, 0x881798) X(DARK_YELLOW, 0xC19C00)                         \
  X(LIGHT_GRAY, 0xCCCCCC) X(DARK_GRAY, 0x767676)                             \
  X(BRIGHT_BLUE, 0x3B78FF) X(BRIGHT_GREEN, 0x16C60C)                         \
  X(BRIGHT_CYAN, 0x61D6D6) X(BRIGHT_RED, 0xE74856)                           \
  X(BRIGHT_MAGENTA, 0xB4009E) X(BRIGHT_YELLOW, 0xF9F1A5) X(WHITE, 0xF2F2F2)

enum {
#define X(NAME, RGB) CNS(Text, Grid_Color, PP_OP_CAT2(WINDOWS_, NAME)) = UINT32_C(RGB),
  TEXT_GRID_WINDOWS_COLOR_TABLE
#undef X
};
enum {
#define X(NAME, RGB) CNS(Text, Grid_Color, PP_OP_CAT2(WINDOWS_INDEX_, NAME)),
  TEXT_GRID_WINDOWS_COLOR_TABLE
#undef X
  CNS(Text, Grid_Color, WINDOWS_COUNT)
};

MODULE(DOMAIN(Modules), Text,
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
        )
    ),
    PACKAGE(Grid_Color,
        FUNCTION(const MODULE_TYPE(Text, Grid_Color, RGB) *, WindowsPalette, ())
    )
);

#include "Impl/Attribute.impl"
