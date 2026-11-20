#ifndef COSMERON_CONTAINER_ARRAY_SPAN_H
#define COSMERON_CONTAINER_ARRAY_SPAN_H

#include "Impl/Common.inc"
#include "../Iterator/Contiguous.h"
#include "Vector.h"
#include "Span.def"

TEMPLATE_INSTANTIATE_TABLE(Array, Span, CONTAINER_ORDERED_TYPE_TABLE);

#endif /* COSMERON_CONTAINER_ARRAY_SPAN_H */
