#pragma once

#include "Common.inc"
#include "Bit.def"
#include "Impl/Bit.impl"

/* ========================================================================== */
/* Standard Width Instantiations                                              */
/* ========================================================================== */

#define COSMERON_MACRO_INTERNAL_BIT_INST(ROW)                                  \
  TEMPLATE_IMPLEMENT(Bit, BIT_WIDTH_GET(SUFFIX, ROW),                          \
                     BIT_WIDTH_GET(TYPE, ROW));

BIT_WIDTH_TABLE(COSMERON_MACRO_INTERNAL_BIT_INST)
#undef COSMERON_MACRO_INTERNAL_BIT_INST

/* EOF */

