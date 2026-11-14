#pragma once

#include "../../../Core/Types/Primitive.h"
#include "MaxAndMin.def"

#define COSMERON_MATH_MAXMIN_ROW(ROW)                                          \
  TEMPLATE_Math_MaxAndMin(PRIMITIVE_TYPE_GET(NAME, ROW),                       \
                          PRIMITIVE_TYPE_GET(TYPE, ROW))

PRIMITIVE_TYPE_TABLE_ORDERED(COSMERON_MATH_MAXMIN_ROW)

#undef COSMERON_MATH_MAXMIN_ROW

/* EOF */
