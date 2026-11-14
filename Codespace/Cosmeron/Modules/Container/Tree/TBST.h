#ifndef COSMERON_CONTAINER_TREE_TBST_H
#define COSMERON_CONTAINER_TREE_TBST_H

#include "Common.inc"
#include "BST.def"

/* Pre-instantiated primitive BSTSet types (ordered types) */
#define COSMERON_MACRO_INTERNAL_TREE_BSTSET_ROW(ROW)                           \
  TEMPLATE_INSTANTIATE(Tree, BSTSet, PRIMITIVE_TYPE_GET(NAME, ROW),            \
                       PRIMITIVE_TYPE_GET(TYPE, ROW));

PRIMITIVE_TYPE_TABLE_ORDERED(COSMERON_MACRO_INTERNAL_TREE_BSTSET_ROW)
#undef COSMERON_MACRO_INTERNAL_TREE_BSTSET_ROW

/* Pre-instantiated common BSTMap types */
TEMPLATE_INSTANTIATE(Tree, BSTMap, Int32_Int32, int32_t, int32_t);
TEMPLATE_INSTANTIATE(Tree, BSTMap, Int32_Float64, int32_t, double);
TEMPLATE_INSTANTIATE(Tree, BSTMap, Size_Size, size_t, size_t);

#endif /* COSMERON_CONTAINER_TREE_TBST_H */
