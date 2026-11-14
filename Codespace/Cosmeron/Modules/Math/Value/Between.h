#pragma once

#include "../../../Core/Types/Primitive.h"
#include "Between.def"

#define COSMERON_MATH_BETWEEN_ROW(ROW)                                         \
  TEMPLATE_Math_Between(PRIMITIVE_TYPE_GET(NAME, ROW),                         \
                        PRIMITIVE_TYPE_GET(TYPE, ROW))

PRIMITIVE_TYPE_TABLE_ORDERED(COSMERON_MATH_BETWEEN_ROW)

#undef COSMERON_MATH_BETWEEN_ROW

/* EOF */
