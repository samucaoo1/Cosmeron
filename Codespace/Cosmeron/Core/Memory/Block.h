#pragma once

#include "Impl/Common.inc"
#include "../Types/Impl/Common.inc"

#if defined(MEMORY_BLOCK_DISABLE_FUNCTION_TABLE) &&                            \
    !defined(COSMERON_DISABLE_FT_Memory_Block)
#define COSMERON_DISABLE_FT_Memory_Block 1
#endif

#include "Block.def"

/* Standard Width Instantiations */
TEMPLATE_INSTANTIATE_TABLE(Memory, Block, TYPE_WIDTH_TYPE_TABLE);

/* EOF */
