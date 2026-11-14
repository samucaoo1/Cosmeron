#ifndef COSMERON_CONTAINER_TREE_TAVL_H
#define COSMERON_CONTAINER_TREE_TAVL_H

#include "Common.inc"
#include "AVL.def"

/* Pre-instantiated primitive AVLSet types (ordered types) */
#define COSMERON_MACRO_INTERNAL_TREE_AVLSET_ROW(ROW)                           \
  TEMPLATE_INSTANTIATE(Tree, AVLSet, PRIMITIVE_TYPE_GET(NAME, ROW),            \
                       PRIMITIVE_TYPE_GET(TYPE, ROW));

PRIMITIVE_TYPE_TABLE_ORDERED(COSMERON_MACRO_INTERNAL_TREE_AVLSET_ROW)
#undef COSMERON_MACRO_INTERNAL_TREE_AVLSET_ROW

/* Pre-instantiated common AVLMap types */
TEMPLATE_INSTANTIATE(Tree, AVLMap, Int32_Int32, int32_t, int32_t);
TEMPLATE_INSTANTIATE(Tree, AVLMap, Int32_Float64, int32_t, double);
TEMPLATE_INSTANTIATE(Tree, AVLMap, Size_Size, size_t, size_t);

#endif /* COSMERON_CONTAINER_TREE_TAVL_H */
