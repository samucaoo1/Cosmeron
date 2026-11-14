#pragma once

#include "Common.inc"
#include "Quad.def"

/* Primitive Type Instantiations */
#define COSMERON_MACRO_INTERNAL_STRUCT_QUAD_ROW(ROW)                           \
  TEMPLATE_INSTANTIATE(Struct, Quad, PRIMITIVE_TYPE_GET(NAME, ROW),            \
                       PRIMITIVE_TYPE_GET(TYPE, ROW));

PRIMITIVE_TYPE_TABLE(COSMERON_MACRO_INTERNAL_STRUCT_QUAD_ROW)
#undef COSMERON_MACRO_INTERNAL_STRUCT_QUAD_ROW

/* EOF */
