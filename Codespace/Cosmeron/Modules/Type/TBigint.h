#pragma once

#include "Common.inc"
#include "Bigint.def"
#include "Impl/Bigint.impl"

/* Standard Width Instantiations */
#define COSMERON_MACRO_INTERNAL_TYPE_BIGINT_INST(ROW)                          \
  TEMPLATE_IMPLEMENT(Type_Bigint, TYPE_WIDTH_GET(SUFFIX, ROW),                 \
                     TYPE_WIDTH_GET(TYPE, ROW));

TYPE_WIDTH_TABLE(COSMERON_MACRO_INTERNAL_TYPE_BIGINT_INST)
#undef COSMERON_MACRO_INTERNAL_TYPE_BIGINT_INST

/* EOF */
