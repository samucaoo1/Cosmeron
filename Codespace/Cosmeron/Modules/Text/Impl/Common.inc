#pragma once

#include <stdint.h>

#include "../../../Core/Module/Module.h"

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

/* EOF */
