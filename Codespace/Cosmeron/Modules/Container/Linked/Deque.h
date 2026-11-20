#ifndef COSMERON_CONTAINER_LINKED_DEQUE_H
#define COSMERON_CONTAINER_LINKED_DEQUE_H

#include "Impl/Common.inc"
#include "../Iterator/Bidirectional.h"
#include "DoubleNode.h"
#include "Deque.def"

TEMPLATE_INSTANTIATE_TABLE(Linked, Deque, CONTAINER_ORDERED_TYPE_TABLE);

#endif /* COSMERON_CONTAINER_LINKED_DEQUE_H */
