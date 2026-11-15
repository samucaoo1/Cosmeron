#ifndef COSMERON_CONTAINER_ITERATOR_TBIDIRECTIONAL_H
#define COSMERON_CONTAINER_ITERATOR_TBIDIRECTIONAL_H

#include "Impl/Common.inc"
#include "../Linked/TDoubleNode.h"
#include "TStream.h"
#include "Bidirectional.def"

TEMPLATE_INSTANTIATE_TABLE(Iterator, Bidirectional,
                           CONTAINER_ORDERED_TYPE_TABLE);

#endif /* COSMERON_CONTAINER_ITERATOR_TBIDIRECTIONAL_H */
