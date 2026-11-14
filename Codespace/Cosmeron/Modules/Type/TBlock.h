#pragma once

#include "Common.inc"
#include "Block.def"
#include "Impl/Block.impl"

/* Standard Width Instantiations */
#define COSMERON_MACRO_INTERNAL_TYPE_BLOCK_INST(ROW)                           \
  TEMPLATE_IMPLEMENT(Type_Block, TYPE_WIDTH_GET(SUFFIX, ROW),                  \
                     TYPE_WIDTH_GET(TYPE, ROW));

TYPE_WIDTH_TABLE(COSMERON_MACRO_INTERNAL_TYPE_BLOCK_INST)
#undef COSMERON_MACRO_INTERNAL_TYPE_BLOCK_INST

/* EOF */
