#pragma once

#if CONTAINER_ENABLE_PSEUDO_METHODS

#define FLAT_QUEUE_FUNCTION_TABLE(TYPE, SUFFIX)                              \
  typedef struct FLAT_QUEUE_FUNC(SUFFIX, FunctionTable) {                   \
    /* Lifecycle */                                                            \
    OPSTATUS (*init)(FLAT_QUEUE_TYPE(SUFFIX) *);                               \
    void (*destroy)(FLAT_QUEUE_TYPE(SUFFIX) *);                                \
    /* Capacity */                                                             \
    OPSTATUS (*reserve)(FLAT_QUEUE_TYPE(SUFFIX) *, size_t);                    \
    OPSTATUS (*shrinkToFit)(FLAT_QUEUE_TYPE(SUFFIX) *);                      \
    OPSTATUS (*compact)(FLAT_QUEUE_TYPE(SUFFIX) *);                            \
    /* Modifiers / Container aliases */                                        \
    OPSTATUS (*push)(FLAT_QUEUE_TYPE(SUFFIX) *, TYPE);                         \
    OPSTATUS (*pop)(FLAT_QUEUE_TYPE(SUFFIX) *, TYPE *);                        \
    OPSTATUS (*pushBack)(FLAT_QUEUE_TYPE(SUFFIX) *, TYPE);                    \
    OPSTATUS (*popFront)(FLAT_QUEUE_TYPE(SUFFIX) *, TYPE *);                  \
    /* Element access */                                                       \
    TYPE *(*front)(FLAT_QUEUE_TYPE(SUFFIX) *);                                 \
    TYPE *(*back)(FLAT_QUEUE_TYPE(SUFFIX) *);                                  \
    /* Observers */                                                            \
    bool (*empty)(const FLAT_QUEUE_TYPE(SUFFIX) *);                                  \
    size_t (*size)(const FLAT_QUEUE_TYPE(SUFFIX) *);                                 \
    size_t (*capacity)(const FLAT_QUEUE_TYPE(SUFFIX) *);                             \
  } FLAT_QUEUE_FUNC(SUFFIX, FunctionTable);

#define FLAT_QUEUE_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)                     \
  static const FLAT_QUEUE_FUNC(SUFFIX, FunctionTable)                              \
      FLAT_QUEUE_FUNC(SUFFIX, functions) = {                                   \
          .init = FLAT_QUEUE_FUNC(SUFFIX, Init),                       \
          .destroy = FLAT_QUEUE_FUNC(SUFFIX, Destroy),                 \
          .reserve = FLAT_QUEUE_FUNC(SUFFIX, Reserve),                 \
          .shrinkToFit = FLAT_QUEUE_FUNC(SUFFIX, ShrinkToFit),     \
          .compact = FLAT_QUEUE_FUNC(SUFFIX, Compact),                 \
          .push = FLAT_QUEUE_FUNC(SUFFIX, Push),                       \
          .pop = FLAT_QUEUE_FUNC(SUFFIX, Pop),                         \
          .pushBack = FLAT_QUEUE_FUNC(SUFFIX, Push),                  \
          .popFront = FLAT_QUEUE_FUNC(SUFFIX, Pop),                   \
          .front = FLAT_QUEUE_FUNC(SUFFIX, Front),                     \
          .back = FLAT_QUEUE_FUNC(SUFFIX, Back),                       \
          .empty = FLAT_QUEUE_FUNC(SUFFIX, Empty),                     \
          .size = FLAT_QUEUE_FUNC(SUFFIX, Size),                       \
          .capacity = FLAT_QUEUE_FUNC(SUFFIX, Capacity)};

#else
#define FLAT_QUEUE_FUNCTION_TABLE(...)
#define FLAT_QUEUE_FUNCTION_TABLE_INSTANCE(...)
#endif
