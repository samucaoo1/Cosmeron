#pragma once

#if COSMERON_MACRO_INTERNAL_CONTAINER_FUNCTION_TABLE_ENABLED

#define FLAT_SPAN_FUNCTION_TABLE_STRUCT(TYPE, SUFFIX)                          \
  typedef struct FLAT_SPAN_FUNCTION_TABLE_TYPE(SUFFIX) {                       \
    /* Lifecycle */                                                            \
    OPSTATUS (*init)(FLAT_SPAN_TYPE(SUFFIX) *);                                \
    OPSTATUS (*fromBuffer)(FLAT_SPAN_TYPE(SUFFIX) *, TYPE *, size_t);          \
    FLAT_SPAN_TYPE(SUFFIX) (*create)(TYPE *, size_t);                          \
    FLAT_SPAN_TYPE(SUFFIX) (*empty)(void);                                     \
    /* Element access */                                                       \
    OPSTATUS (*at)(FLAT_SPAN_TYPE(SUFFIX) *, size_t, TYPE **);                \
    OPSTATUS (*front)(FLAT_SPAN_TYPE(SUFFIX) *, TYPE **);                     \
    OPSTATUS (*back)(FLAT_SPAN_TYPE(SUFFIX) *, TYPE **);                      \
    OPSTATUS (*data)(FLAT_SPAN_TYPE(SUFFIX) *, TYPE **);                      \
    /* Slicing */                                                              \
    OPSTATUS (*subspan)(const FLAT_SPAN_TYPE(SUFFIX) *, size_t, size_t,        \
                        FLAT_SPAN_TYPE(SUFFIX) *);                             \
    OPSTATUS (*first)(const FLAT_SPAN_TYPE(SUFFIX) *, size_t,                  \
                      FLAT_SPAN_TYPE(SUFFIX) *);                               \
    OPSTATUS (*last)(const FLAT_SPAN_TYPE(SUFFIX) *, size_t,                   \
                     FLAT_SPAN_TYPE(SUFFIX) *);                                \
    /* Observers */                                                            \
    size_t (*size)(const FLAT_SPAN_TYPE(SUFFIX) *);                            \
    size_t (*sizeBytes)(const FLAT_SPAN_TYPE(SUFFIX) *);                       \
    bool (*isEmpty)(const FLAT_SPAN_TYPE(SUFFIX) *);                           \
    /* Comparison */                                                           \
    bool (*equal)(const FLAT_SPAN_TYPE(SUFFIX) *,                              \
                  const FLAT_SPAN_TYPE(SUFFIX) *);                             \
    /* Interoperability */                                                     \
    OPSTATUS (*fromVector)(FLAT_SPAN_TYPE(SUFFIX) *,                           \
                           const FLAT_VECTOR_TYPE(SUFFIX) *);                  \
  } FLAT_SPAN_FUNCTION_TABLE_TYPE(SUFFIX);

#define FLAT_SPAN_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)                        \
  static const FLAT_SPAN_FUNCTION_TABLE_TYPE(SUFFIX)                           \
      FLAT_SPAN_FUNC(SUFFIX, functions) = {                                    \
          .init = FLAT_SPAN_FUNC(SUFFIX, Init),                                \
          .fromBuffer = FLAT_SPAN_FUNC(SUFFIX, FromBuffer),                    \
          .create = FLAT_SPAN_FUNC(SUFFIX, Create),                            \
          .empty = FLAT_SPAN_FUNC(SUFFIX, Empty),                              \
          .at = FLAT_SPAN_FUNC(SUFFIX, At),                                    \
          .front = FLAT_SPAN_FUNC(SUFFIX, Front),                              \
          .back = FLAT_SPAN_FUNC(SUFFIX, Back),                                \
          .data = FLAT_SPAN_FUNC(SUFFIX, Data),                                \
          .subspan = FLAT_SPAN_FUNC(SUFFIX, Subspan),                          \
          .first = FLAT_SPAN_FUNC(SUFFIX, First),                              \
          .last = FLAT_SPAN_FUNC(SUFFIX, Last),                                \
          .size = FLAT_SPAN_FUNC(SUFFIX, Size),                                \
          .sizeBytes = FLAT_SPAN_FUNC(SUFFIX, SizeBytes),                      \
          .isEmpty = FLAT_SPAN_FUNC(SUFFIX, IsEmpty),                          \
          .equal = FLAT_SPAN_FUNC(SUFFIX, Equal),                              \
          .fromVector = FLAT_SPAN_FUNC(SUFFIX, FromVector)};

#else
#define FLAT_SPAN_FUNCTION_TABLE_STRUCT(...)
#define FLAT_SPAN_FUNCTION_TABLE_INSTANCE(...)
#endif
