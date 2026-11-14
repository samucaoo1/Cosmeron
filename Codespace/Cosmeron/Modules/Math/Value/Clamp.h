#pragma once

#include "../../../Core/Types/Primitive.h"
#include "Clamp.def"

#define COSMERON_MATH_CLAMP_ROW(ROW)                                           \
  TEMPLATE_Math_Clamp(PRIMITIVE_TYPE_GET(NAME, ROW),                           \
                      PRIMITIVE_TYPE_GET(TYPE, ROW))

PRIMITIVE_TYPE_TABLE_ORDERED(COSMERON_MATH_CLAMP_ROW)

#undef COSMERON_MATH_CLAMP_ROW

/* EOF */
