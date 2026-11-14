#ifndef COSMERON_CONTAINER_TREE_TREDBLACK_H
#define COSMERON_CONTAINER_TREE_TREDBLACK_H

#include "Common.inc"
#include "RedBlack.def"

/* Pre-instantiated primitive RBSet types (ordered types) */
#define COSMERON_MACRO_INTERNAL_TREE_RBSET_ROW(ROW)                            \
  TEMPLATE_INSTANTIATE(Tree, RBSet, PRIMITIVE_TYPE_GET(NAME, ROW),             \
                       PRIMITIVE_TYPE_GET(TYPE, ROW));

PRIMITIVE_TYPE_TABLE_ORDERED(COSMERON_MACRO_INTERNAL_TREE_RBSET_ROW)
#undef COSMERON_MACRO_INTERNAL_TREE_RBSET_ROW

/* Pre-instantiated common RBMap types */
TEMPLATE_INSTANTIATE(Tree, RBMap, Int32_Int32, int32_t, int32_t);
TEMPLATE_INSTANTIATE(Tree, RBMap, Int32_Float64, int32_t, double);
TEMPLATE_INSTANTIATE(Tree, RBMap, Size_Size, size_t, size_t);

#endif /* COSMERON_CONTAINER_TREE_TREDBLACK_H */
