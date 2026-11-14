#pragma once

#include "Impl/Common.inc"
#include "Quad.def"

/* Primitive Type Instantiations */
#define COSMERON_MACRO_INTERNAL_TUPLE_QUAD_ROW(ROW)                           \
  TEMPLATE_INSTANTIATE(Tuple, Quad, PRIMITIVE_TYPE_GET(NAME, ROW),            \
                       PRIMITIVE_TYPE_GET(TYPE, ROW));

PRIMITIVE_TYPE_TABLE(COSMERON_MACRO_INTERNAL_TUPLE_QUAD_ROW)
#undef COSMERON_MACRO_INTERNAL_TUPLE_QUAD_ROW

/* EOF */
