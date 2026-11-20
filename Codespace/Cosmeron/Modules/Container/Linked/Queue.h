#ifndef COSMERON_CONTAINER_LINKED_QUEUE_H
#define COSMERON_CONTAINER_LINKED_QUEUE_H

#include "Impl/Common.inc"
#include "../Iterator/Forward.h"
#include "SingleNode.h"
#include "Queue.def"

TEMPLATE_INSTANTIATE_TABLE(Linked, Queue, CONTAINER_ORDERED_TYPE_TABLE);

#endif /* COSMERON_CONTAINER_LINKED_QUEUE_H */
