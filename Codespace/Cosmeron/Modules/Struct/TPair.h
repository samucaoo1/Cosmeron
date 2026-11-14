#pragma once

#include "Common.inc"
#include "Pair.def"

/* Primitive Type Instantiations */
#define COSMERON_MACRO_INTERNAL_STRUCT_PAIR_ROW(ROW)                           \
  TEMPLATE_INSTANTIATE(Struct, Pair, PRIMITIVE_TYPE_GET(NAME, ROW),            \
                       PRIMITIVE_TYPE_GET(TYPE, ROW));

PRIMITIVE_TYPE_TABLE(COSMERON_MACRO_INTERNAL_STRUCT_PAIR_ROW)
#undef COSMERON_MACRO_INTERNAL_STRUCT_PAIR_ROW

/* EOF */
