#ifndef COSMERON_CONTAINER_HASH_THASHMAP_H
#define COSMERON_CONTAINER_HASH_THASHMAP_H

#include "Common.inc"
#include "Map.def"

/* Pre-instantiated common HashMap types */
TEMPLATE_INSTANTIATE(Hash, HashMap, Int32_Int32, int32_t, int32_t);
TEMPLATE_INSTANTIATE(Hash, HashMap, Int32_Float64, int32_t, double);
TEMPLATE_INSTANTIATE(Hash, HashMap, Size_Size, size_t, size_t);

#endif /* COSMERON_CONTAINER_HASH_THASHMAP_H */

