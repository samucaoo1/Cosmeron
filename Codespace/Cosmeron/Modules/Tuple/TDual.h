#pragma once

#include "Impl/Common.inc"
#include "Dual.def"

/* Primitive Type Instantiations */
#define COSMERON_MACRO_INTERNAL_TUPLE_DUAL_ROW(ROW)                           \
  TEMPLATE_INSTANTIATE(Tuple, Dual, PRIMITIVE_TYPE_GET(NAME, ROW),            \
                       PRIMITIVE_TYPE_GET(TYPE, ROW));

PRIMITIVE_TYPE_TABLE(COSMERON_MACRO_INTERNAL_TUPLE_DUAL_ROW)
#undef COSMERON_MACRO_INTERNAL_TUPLE_DUAL_ROW

/* EOF */
