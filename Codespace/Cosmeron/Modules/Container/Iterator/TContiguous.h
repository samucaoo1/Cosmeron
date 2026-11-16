#ifndef COSMERON_CONTAINER_ITERATOR_TCONTIGUOUS_H
#define COSMERON_CONTAINER_ITERATOR_TCONTIGUOUS_H

#include "Impl/Common.inc"
#include "TStream.h"
#include "Contiguous.def"

#define COSMERON_INTERNAL_ITERATOR_CONTIGUOUS_STREAM_PROTO_I(SUFFIX, TYPE) \
  static OPSTATUS NS(Iterator, Contiguous, SUFFIX, StreamNext)(void *state, TYPE *outValue); \
  static bool NS(Iterator, Contiguous, SUFFIX, StreamHasNext)(const void *state); \
  static OPSTATUS NS(Iterator, Contiguous, SUFFIX, StreamReset)(void *state);
#define COSMERON_INTERNAL_ITERATOR_CONTIGUOUS_STREAM_PROTO(ROW) \
  COSMERON_INTERNAL_ITERATOR_CONTIGUOUS_STREAM_PROTO_I( \
      PP_SEQ_GET_1 ROW, PP_SEQ_GET_2 ROW)
CONTAINER_ORDERED_TYPE_TABLE(COSMERON_INTERNAL_ITERATOR_CONTIGUOUS_STREAM_PROTO)
#undef COSMERON_INTERNAL_ITERATOR_CONTIGUOUS_STREAM_PROTO
#undef COSMERON_INTERNAL_ITERATOR_CONTIGUOUS_STREAM_PROTO_I

TEMPLATE_INSTANTIATE_TABLE(Iterator, Contiguous, CONTAINER_ORDERED_TYPE_TABLE);

#define COSMERON_INTERNAL_ITERATOR_CONTIGUOUS_STREAM_ADAPTER_I(SUFFIX, TYPE) \
  static OPSTATUS NS(Iterator, Contiguous, SUFFIX, StreamNext)( \
      void *state, TYPE *outValue) { \
    if (!state || !outValue) \
      return STATUS_CONST(INVALID_ARGUMENT); \
    return MODULE_FUNC(Iterator, Contiguous, SUFFIX, StepValue)( \
        (MODULE_TYPED(Iterator, Contiguous, SUFFIX) *)state, outValue); \
  } \
  static bool NS(Iterator, Contiguous, SUFFIX, StreamHasNext)(const void *state) { \
    if (!state) \
      return false; \
    return MODULE_FUNC(Iterator, Contiguous, SUFFIX, HasNext)( \
        (const MODULE_TYPED(Iterator, Contiguous, SUFFIX) *)state); \
  } \
  static OPSTATUS NS(Iterator, Contiguous, SUFFIX, StreamReset)(void *state) { \
    if (!state) \
      return STATUS_CONST(INVALID_ARGUMENT); \
    return MODULE_FUNC(Iterator, Contiguous, SUFFIX, Reset)( \
        (MODULE_TYPED(Iterator, Contiguous, SUFFIX) *)state); \
  }
#define COSMERON_INTERNAL_ITERATOR_CONTIGUOUS_STREAM_ADAPTER(ROW) \
  COSMERON_INTERNAL_ITERATOR_CONTIGUOUS_STREAM_ADAPTER_I( \
      PP_SEQ_GET_1 ROW, PP_SEQ_GET_2 ROW)
CONTAINER_ORDERED_TYPE_TABLE(COSMERON_INTERNAL_ITERATOR_CONTIGUOUS_STREAM_ADAPTER)
#undef COSMERON_INTERNAL_ITERATOR_CONTIGUOUS_STREAM_ADAPTER
#undef COSMERON_INTERNAL_ITERATOR_CONTIGUOUS_STREAM_ADAPTER_I

#endif /* COSMERON_CONTAINER_ITERATOR_TCONTIGUOUS_H */
