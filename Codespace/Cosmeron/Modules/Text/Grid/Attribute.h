#pragma once

#include "../../../Core/Error/Status.h"
#include "../../../Core/Memory/Alloc.h"
#include "../../Struct/TDual.h"
#include "../../Struct/TQuad.h"
#include "../Text.space"

typedef uint16_t TEXT_GRID_ATTRIBUTE_TYPE(Flags);
typedef uint8_t TEXT_GRID_UNDERLINE_TYPE(Style);
typedef uint32_t TEXT_GRID_COLOR_TYPE(RGB);

enum {
  TEXT_GRID_ATTRIBUTE_CONST(BOLD) = UINT16_C(1) << 0U,
  TEXT_GRID_ATTRIBUTE_CONST(FAINT) = UINT16_C(1) << 1U,
  TEXT_GRID_ATTRIBUTE_CONST(ITALIC) = UINT16_C(1) << 2U,
  TEXT_GRID_ATTRIBUTE_CONST(BLINK) = UINT16_C(1) << 3U,
  TEXT_GRID_ATTRIBUTE_CONST(INVERSE) = UINT16_C(1) << 4U,
  TEXT_GRID_ATTRIBUTE_CONST(HIDDEN) = UINT16_C(1) << 5U,
  TEXT_GRID_ATTRIBUTE_CONST(STRIKE) = UINT16_C(1) << 6U,
  TEXT_GRID_ATTRIBUTE_CONST(OVERLINE) = UINT16_C(1) << 7U,
  TEXT_GRID_ATTRIBUTE_CONST(TRANSPARENT_BACKGROUND) = UINT16_C(1) << 8U,
  TEXT_GRID_ATTRIBUTE_CONST(CODE_UNIT_CONTINUATION) = UINT16_C(1) << 9U,
  TEXT_GRID_ATTRIBUTE_CONST(DEFAULT_FOREGROUND) = UINT16_C(1) << 10U,
  TEXT_GRID_ATTRIBUTE_CONST(DEFAULT_BACKGROUND) = UINT16_C(1) << 11U,
  TEXT_GRID_ATTRIBUTE_CONST(RESERVED) = UINT16_C(1) << 12U
};

enum {
  TEXT_GRID_UNDERLINE_CONST(NONE) = 0,
  TEXT_GRID_UNDERLINE_CONST(SINGLE),
  TEXT_GRID_UNDERLINE_CONST(DOUBLE),
  TEXT_GRID_UNDERLINE_CONST(CURLY),
  TEXT_GRID_UNDERLINE_CONST(DOTTED),
  TEXT_GRID_UNDERLINE_CONST(DASHED),
  TEXT_GRID_UNDERLINE_CONST(RESERVED_1),
  TEXT_GRID_UNDERLINE_CONST(RESERVED_2)
};

typedef struct {
  uint32_t foreground : 24;
  uint32_t background : 24;
  uint16_t flags : 13;
  uint16_t underline : 3;
} TEXT_GRID_ATTRIBUTE_TYPE(Cell);

typedef struct {
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) *data;
  TDUAL_TYPE(uint16) size;
} TEXT_GRID_ATTRIBUTE_TYPE(Grid);

enum { TEXT_GRID_COLOR_CONST(RGB_MASK) = UINT32_C(0x00FFFFFF) };
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
#define X(NAME, RGB) TEXT_GRID_COLOR_CONST(PP_OP_CAT2(WINDOWS_, NAME)) = UINT32_C(RGB),
  TEXT_GRID_WINDOWS_COLOR_TABLE
#undef X
};
enum {
#define X(NAME, RGB) TEXT_GRID_COLOR_CONST(PP_OP_CAT2(WINDOWS_INDEX_, NAME)),
  TEXT_GRID_WINDOWS_COLOR_TABLE
#undef X
  TEXT_GRID_COLOR_CONST(WINDOWS_COUNT)
};

static inline TEXT_GRID_ATTRIBUTE_TYPE(Cell)
TEXT_GRID_ATTRIBUTE_FUNC(Default)(void);
static inline const TEXT_GRID_COLOR_TYPE(RGB) *
TEXT_GRID_COLOR_FUNC(WindowsPalette)(void);
static inline bool TEXT_GRID_ATTRIBUTE_FUNC(IsEmpty)(
    const TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid);
static inline bool TEXT_GRID_ATTRIBUTE_FUNC(IsValid)(
    const TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid);
static inline OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(Create)(
    TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid, TDUAL_TYPE(uint16) size);
static inline OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(Destroy)(
    TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid);
static inline OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(Resize)(
    TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid, TDUAL_TYPE(uint16) size);
static inline OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(Recreate)(
    TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid, TDUAL_TYPE(uint16) size);
static inline OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(Clone)(
    TEXT_GRID_ATTRIBUTE_TYPE(Grid) *destination,
    const TEXT_GRID_ATTRIBUTE_TYPE(Grid) *source);
static inline OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(Clear)(
    TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid);
static inline OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(ClearWith)(
    TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid,
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute);
static inline OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(Fill)(
    TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid,
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute);
static inline OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(ReadCell)(
    const TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid, TDUAL_TYPE(uint16) position,
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) *attribute);
static inline OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(WriteCell)(
    TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid, TDUAL_TYPE(uint16) position,
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute);
static inline OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(FillRegion)(
    TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid, TQUAD_TYPE(uint16) region,
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute);
static inline OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(Read)(
    const TEXT_GRID_ATTRIBUTE_TYPE(Grid) *source, TQUAD_TYPE(uint16) region,
    TEXT_GRID_ATTRIBUTE_TYPE(Grid) *destination);
static inline OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(Write)(
    TEXT_GRID_ATTRIBUTE_TYPE(Grid) *destination,
    TDUAL_TYPE(uint16) destinationPosition,
    const TEXT_GRID_ATTRIBUTE_TYPE(Grid) *source);
static inline OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(WriteRegion)(
    TEXT_GRID_ATTRIBUTE_TYPE(Grid) *destination,
    TDUAL_TYPE(uint16) destinationPosition,
    const TEXT_GRID_ATTRIBUTE_TYPE(Grid) *source,
    TQUAD_TYPE(uint16) sourceRegion);

#include "Impl/Attribute.impl"
