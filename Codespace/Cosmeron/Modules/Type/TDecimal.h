#pragma once

#include "Common.inc"
#include "Decimal.def"
#include "Impl/Decimal.impl"

/* Standard Width Instantiations */
#define COSMERON_MACRO_INTERNAL_TYPE_DECIMAL_INST(ROW)                         \
  TEMPLATE_IMPLEMENT(Type_Decimal, TYPE_WIDTH_GET(SUFFIX, ROW),                \
                     TYPE_WIDTH_GET(TYPE, ROW));

TYPE_WIDTH_TABLE(COSMERON_MACRO_INTERNAL_TYPE_DECIMAL_INST)
#undef COSMERON_MACRO_INTERNAL_TYPE_DECIMAL_INST

/* EOF */
