#pragma once

#include "Impl/Common.inc"
#include "../Types/Impl/Common.inc"

#if defined(BITSET_DISABLE_FUNCTION_TABLE) &&                                  \
    !defined(COSMERON_DISABLE_FT_Bitset)
#define COSMERON_DISABLE_FT_Bitset 1
#endif

#include "Bitset.def"

/* Standard Width Instantiations */
TEMPLATE_INSTANTIATE_TABLE(Bitset, TYPE_BITSET_WIDTH_TABLE);

/* EOF */
