#ifndef COSMERON_CONTAINER_TREE_TBTREE_H
#define COSMERON_CONTAINER_TREE_TBTREE_H

#include "Common.inc"
#include "BTree.def"

/* Pre-instantiated primitive BTreeSet types (ordered types) */
#define COSMERON_MACRO_INTERNAL_TREE_BTREESET_ROW(ROW)                         \
  TEMPLATE_INSTANTIATE(Tree, BTreeSet, PRIMITIVE_TYPE_GET(NAME, ROW),          \
                       PRIMITIVE_TYPE_GET(TYPE, ROW));

PRIMITIVE_TYPE_TABLE_ORDERED(COSMERON_MACRO_INTERNAL_TREE_BTREESET_ROW)
#undef COSMERON_MACRO_INTERNAL_TREE_BTREESET_ROW

/* Pre-instantiated common BTreeMap types */
TEMPLATE_INSTANTIATE(Tree, BTreeMap, Int32_Int32, int32_t, int32_t);
TEMPLATE_INSTANTIATE(Tree, BTreeMap, Int32_Float64, int32_t, double);
TEMPLATE_INSTANTIATE(Tree, BTreeMap, Size_Size, size_t, size_t);

#endif /* COSMERON_CONTAINER_TREE_TBTREE_H */
