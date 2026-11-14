#pragma once

#include "Impl/Common.inc"
#include "Triple.def"

/* Primitive Type Instantiations */
#define COSMERON_MACRO_INTERNAL_TUPLE_TRIPLE_ROW(ROW)                         \
  TEMPLATE_INSTANTIATE(Tuple, Triple, PRIMITIVE_TYPE_GET(NAME, ROW),          \
                       PRIMITIVE_TYPE_GET(TYPE, ROW));

PRIMITIVE_TYPE_TABLE(COSMERON_MACRO_INTERNAL_TUPLE_TRIPLE_ROW)
#undef COSMERON_MACRO_INTERNAL_TUPLE_TRIPLE_ROW

/* EOF */
