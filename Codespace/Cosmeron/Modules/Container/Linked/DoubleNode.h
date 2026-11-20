#ifndef COSMERON_CONTAINER_LINKED_TDOUBLENODE_H
#define COSMERON_CONTAINER_LINKED_TDOUBLENODE_H

#include "../../../Core/Module/Module.h"
#include "../Impl/Common.inc"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "DoubleNode.def"

TEMPLATE_INSTANTIATE_TABLE(Linked, DoubleNode, CONTAINER_ORDERED_TYPE_TABLE);

#endif /* COSMERON_CONTAINER_LINKED_TDOUBLENODE_H */
