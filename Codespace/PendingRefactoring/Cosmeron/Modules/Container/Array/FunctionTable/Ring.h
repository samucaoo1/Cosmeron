#pragma once

#if COSMERON_MACRO_INTERNAL_CONTAINER_FUNCTION_TABLE_ENABLED

#define FLAT_RING_FUNCTION_TABLE_STRUCT(TYPE, SUFFIX)                          \
  typedef struct FLAT_RING_FUNCTION_TABLE_TYPE(SUFFIX) {                       \
    /* Lifecycle */                                                            \
    OPSTATUS (*init)(FLAT_RING_TYPE(SUFFIX) *, TYPE *, size_t);                \
    void (*destroy)(FLAT_RING_TYPE(SUFFIX) *);                                 \
    void (*clear)(FLAT_RING_TYPE(SUFFIX) *);                                   \
    /* Modifiers */                                                            \
    OPSTATUS (*push)(FLAT_RING_TYPE(SUFFIX) *, TYPE);                          \
    OPSTATUS (*pop)(FLAT_RING_TYPE(SUFFIX) *, TYPE *);                         \
    OPSTATUS (*peek)(const FLAT_RING_TYPE(SUFFIX) *, TYPE *);                  \
    OPSTATUS (*overwrite)(FLAT_RING_TYPE(SUFFIX) *, TYPE, TYPE *);             \
    /* Bulk I/O */                                                             \
    OPSTATUS (*write)(FLAT_RING_TYPE(SUFFIX) *, const TYPE *, size_t,          \
                      size_t *);                                               \
    OPSTATUS (*read)(FLAT_RING_TYPE(SUFFIX) *, TYPE *, size_t, size_t *);      \
    /* Zero-copy Spans */                                                      \
    OPSTATUS (*getReadSpan)(FLAT_RING_TYPE(SUFFIX) *, TYPE **, size_t *);      \
    OPSTATUS (*advanceRead)(FLAT_RING_TYPE(SUFFIX) *, size_t);                 \
    OPSTATUS (*getWriteSpan)(FLAT_RING_TYPE(SUFFIX) *, TYPE **, size_t *);     \
    OPSTATUS (*advanceWrite)(FLAT_RING_TYPE(SUFFIX) *, size_t);                \
    /* Element access */                                                       \
    OPSTATUS (*front)(FLAT_RING_TYPE(SUFFIX) *, TYPE **);                      \
    OPSTATUS (*back)(FLAT_RING_TYPE(SUFFIX) *, TYPE **);                       \
    OPSTATUS (*at)(FLAT_RING_TYPE(SUFFIX) *, size_t, TYPE **);                 \
    /* Observers */                                                            \
    bool (*empty)(const FLAT_RING_TYPE(SUFFIX) *);                             \
    bool (*full)(const FLAT_RING_TYPE(SUFFIX) *);                              \
    size_t (*size)(const FLAT_RING_TYPE(SUFFIX) *);                            \
    size_t (*capacity)(const FLAT_RING_TYPE(SUFFIX) *);                        \
    size_t (*available)(const FLAT_RING_TYPE(SUFFIX) *);                       \
  } FLAT_RING_FUNCTION_TABLE_TYPE(SUFFIX);

#define FLAT_RING_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)                        \
  static const FLAT_RING_FUNCTION_TABLE_TYPE(SUFFIX)                           \
      FLAT_RING_FUNC(SUFFIX, functions) = {                                    \
          .init = FLAT_RING_FUNC(SUFFIX, Init),                                \
          .destroy = FLAT_RING_FUNC(SUFFIX, Destroy),                          \
          .clear = FLAT_RING_FUNC(SUFFIX, Clear),                              \
          .push = FLAT_RING_FUNC(SUFFIX, Push),                                \
          .pop = FLAT_RING_FUNC(SUFFIX, Pop),                                  \
          .peek = FLAT_RING_FUNC(SUFFIX, Peek),                                \
          .overwrite = FLAT_RING_FUNC(SUFFIX, Overwrite),                      \
          .write = FLAT_RING_FUNC(SUFFIX, Write),                              \
          .read = FLAT_RING_FUNC(SUFFIX, Read),                                \
          .getReadSpan = FLAT_RING_FUNC(SUFFIX, GetReadSpan),                  \
          .advanceRead = FLAT_RING_FUNC(SUFFIX, AdvanceRead),                  \
          .getWriteSpan = FLAT_RING_FUNC(SUFFIX, GetWriteSpan),                \
          .advanceWrite = FLAT_RING_FUNC(SUFFIX, AdvanceWrite),                \
          .front = FLAT_RING_FUNC(SUFFIX, Front),                              \
          .back = FLAT_RING_FUNC(SUFFIX, Back),                                \
          .at = FLAT_RING_FUNC(SUFFIX, At),                                    \
          .empty = FLAT_RING_FUNC(SUFFIX, Empty),                              \
          .full = FLAT_RING_FUNC(SUFFIX, Full),                                \
          .size = FLAT_RING_FUNC(SUFFIX, Size),                                \
          .capacity = FLAT_RING_FUNC(SUFFIX, Capacity),                        \
          .available = FLAT_RING_FUNC(SUFFIX, Available)};

#else
#define FLAT_RING_FUNCTION_TABLE_STRUCT(...)
#define FLAT_RING_FUNCTION_TABLE_INSTANCE(...)
#endif
