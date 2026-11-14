#pragma once

#include "Common.inc"
#include "Penta.def"

/* Primitive Type Instantiations */
#define COSMERON_MACRO_INTERNAL_STRUCT_PENTA_ROW(ROW)                          \
  TEMPLATE_INSTANTIATE(Struct, Penta, PRIMITIVE_TYPE_GET(NAME, ROW),           \
                       PRIMITIVE_TYPE_GET(TYPE, ROW));

PRIMITIVE_TYPE_TABLE(COSMERON_MACRO_INTERNAL_STRUCT_PENTA_ROW)
#undef COSMERON_MACRO_INTERNAL_STRUCT_PENTA_ROW

/* EOF */
