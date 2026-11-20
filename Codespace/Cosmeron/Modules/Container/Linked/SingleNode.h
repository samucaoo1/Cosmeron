#ifndef COSMERON_CONTAINER_LINKED_TSINGLENODE_H
#define COSMERON_CONTAINER_LINKED_TSINGLENODE_H

#include "../../../Core/Module/Module.h"
#include "../Impl/Common.inc"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "SingleNode.def"

TEMPLATE_INSTANTIATE_TABLE(Linked, SingleNode, CONTAINER_ORDERED_TYPE_TABLE);

#endif /* COSMERON_CONTAINER_LINKED_TSINGLENODE_H */
