#pragma once

#if COSMERON_MACRO_INTERNAL_CONTAINER_FUNCTION_TABLE_ENABLED

#define LINKED_RING_FUNCTION_TABLE_STRUCT(TYPE, SUFFIX)                        \
  typedef struct LINKED_RING_FUNCTION_TABLE_TYPE(SUFFIX) {                     \
    /* Lifecycle */                                                            \
    OPSTATUS (*init)(LINKED_RING_TYPE(SUFFIX) *, size_t);                      \
    OPSTATUS (*initStatic)(LINKED_RING_TYPE(SUFFIX) *,                         \
                           LINKED_RING_NODE_TYPE(SUFFIX) *, size_t);           \
    void (*destroy)(LINKED_RING_TYPE(SUFFIX) *);                               \
    void (*clear)(LINKED_RING_TYPE(SUFFIX) *);                                 \
    /* Modifiers */                                                            \
    OPSTATUS (*push)(LINKED_RING_TYPE(SUFFIX) *, TYPE);                        \
    OPSTATUS (*pop)(LINKED_RING_TYPE(SUFFIX) *, TYPE *);                       \
    OPSTATUS (*peek)(const LINKED_RING_TYPE(SUFFIX) *, TYPE *);                \
    OPSTATUS (*overwrite)(LINKED_RING_TYPE(SUFFIX) *, TYPE, TYPE *);           \
    /* Element access */                                                       \
    OPSTATUS (*front)(LINKED_RING_TYPE(SUFFIX) *, TYPE **);                    \
    OPSTATUS (*back)(LINKED_RING_TYPE(SUFFIX) *, TYPE **);                     \
    OPSTATUS (*at)(LINKED_RING_TYPE(SUFFIX) *, size_t, TYPE **);               \
    /* Observers */                                                            \
    bool (*empty)(const LINKED_RING_TYPE(SUFFIX) *);                           \
    bool (*full)(const LINKED_RING_TYPE(SUFFIX) *);                            \
    size_t (*size)(const LINKED_RING_TYPE(SUFFIX) *);                          \
    size_t (*capacity)(const LINKED_RING_TYPE(SUFFIX) *);                      \
    size_t (*available)(const LINKED_RING_TYPE(SUFFIX) *);                     \
  } LINKED_RING_FUNCTION_TABLE_TYPE(SUFFIX);

#define LINKED_RING_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)                      \
  static const LINKED_RING_FUNCTION_TABLE_TYPE(SUFFIX)                         \
      LINKED_RING_FUNC(SUFFIX, functions) = {                                  \
          .init = LINKED_RING_FUNC(SUFFIX, Init),                              \
          .initStatic = LINKED_RING_FUNC(SUFFIX, InitStatic),                  \
          .destroy = LINKED_RING_FUNC(SUFFIX, Destroy),                        \
          .clear = LINKED_RING_FUNC(SUFFIX, Clear),                            \
          .push = LINKED_RING_FUNC(SUFFIX, Push),                              \
          .pop = LINKED_RING_FUNC(SUFFIX, Pop),                                \
          .peek = LINKED_RING_FUNC(SUFFIX, Peek),                              \
          .overwrite = LINKED_RING_FUNC(SUFFIX, Overwrite),                    \
          .front = LINKED_RING_FUNC(SUFFIX, Front),                            \
          .back = LINKED_RING_FUNC(SUFFIX, Back),                              \
          .at = LINKED_RING_FUNC(SUFFIX, At),                                  \
          .empty = LINKED_RING_FUNC(SUFFIX, Empty),                            \
          .full = LINKED_RING_FUNC(SUFFIX, Full),                              \
          .size = LINKED_RING_FUNC(SUFFIX, Size),                              \
          .capacity = LINKED_RING_FUNC(SUFFIX, Capacity),                      \
          .available = LINKED_RING_FUNC(SUFFIX, Available)};

#else
#define LINKED_RING_FUNCTION_TABLE_STRUCT(...)
#define LINKED_RING_FUNCTION_TABLE_INSTANCE(...)
#endif
