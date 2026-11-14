#pragma once

#include "Common.inc"
#include "Bitset.def"
#include "Impl/Bitset.impl"

/* Standard Width Instantiations */
#define COSMERON_MACRO_INTERNAL_TYPE_BITSET_INST(ROW)                          \
  TEMPLATE_IMPLEMENT(Type_Bitset, TYPE_WIDTH_GET(SUFFIX, ROW),                 \
                     TYPE_WIDTH_GET(SUFFIX, ROW));

TYPE_WIDTH_TABLE(COSMERON_MACRO_INTERNAL_TYPE_BITSET_INST)
#undef COSMERON_MACRO_INTERNAL_TYPE_BITSET_INST

/* EOF */

