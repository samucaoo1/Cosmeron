#ifndef COSMERON_CONTAINER_LINKED_TSINGLENODE_H
#define COSMERON_CONTAINER_LINKED_TSINGLENODE_H

#include "../../../Core/Module/Module.h"
#include "../../../Core/Primitive/Types.h"

#ifndef TEMPLATE_Linked
#define TEMPLATE_Linked(TARGET, ...) NS(TEMPLATE_Linked, TARGET)(__VA_ARGS__)
#endif

#include "SingleNode.def"

/* Ordered Primitive Type Instantiations */
#define COSMERON_MACRO_INTERNAL_LINKED_SINGLE_NODE_ROW(ROW)                    \
  TEMPLATE_INSTANTIATE(Linked, SingleNode, PRIMITIVE_TYPE_GET(NAME, ROW),      \
                       PRIMITIVE_TYPE_GET(TYPE, ROW));

PRIMITIVE_TYPE_TABLE_ORDERED(COSMERON_MACRO_INTERNAL_LINKED_SINGLE_NODE_ROW)
#undef COSMERON_MACRO_INTERNAL_LINKED_SINGLE_NODE_ROW

#endif /* COSMERON_CONTAINER_LINKED_TSINGLENODE_H */

