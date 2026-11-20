#ifndef COSMERON_CONTAINER_ITERATOR_BIDIRECTIONAL_H
#define COSMERON_CONTAINER_ITERATOR_BIDIRECTIONAL_H

#include "Impl/Common.inc"
#include "../Linked/DoubleNode.h"
#include "Stream.h"
#include "Bidirectional.def"

TEMPLATE_INSTANTIATE_TABLE(Iterator, Bidirectional,
                           CONTAINER_ORDERED_TYPE_TABLE);

#endif /* COSMERON_CONTAINER_ITERATOR_BIDIRECTIONAL_H */
