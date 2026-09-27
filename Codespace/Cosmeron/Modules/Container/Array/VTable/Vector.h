#pragma once

#if CONTAINER_ENABLE_PSEUDO_METHODS

#define FLAT_VECTOR_FUNCTION_TABLE(TYPE, SUFFIX)                             \
  typedef struct FLAT_VECTOR_FUNC(SUFFIX, FunctionTable) {                  \
    /* Lifecycle */                                                            \
    OPSTATUS (*init)(FLAT_VECTOR_TYPE(SUFFIX) *);                              \
    void (*destroy)(FLAT_VECTOR_TYPE(SUFFIX) *);                               \
    /* Capacity */                                                             \
    OPSTATUS (*reserve)(FLAT_VECTOR_TYPE(SUFFIX) *, size_t);                   \
    OPSTATUS (*shrinkToFit)(FLAT_VECTOR_TYPE(SUFFIX) *);                     \
    /* Element access */                                                       \
    TYPE *(*at)(FLAT_VECTOR_TYPE(SUFFIX) *, size_t);                           \
    TYPE *(*front)(FLAT_VECTOR_TYPE(SUFFIX) *);                                \
    TYPE *(*back)(FLAT_VECTOR_TYPE(SUFFIX) *);                                 \
    TYPE *(*data)(FLAT_VECTOR_TYPE(SUFFIX) *);                                 \
    /* Modifiers */                                                            \
    OPSTATUS (*pushBack)(FLAT_VECTOR_TYPE(SUFFIX) *, TYPE);                   \
    OPSTATUS (*popBack)(FLAT_VECTOR_TYPE(SUFFIX) *, TYPE *);                  \
    OPSTATUS (*insert)(FLAT_VECTOR_TYPE(SUFFIX) *, size_t, TYPE);              \
    OPSTATUS (*erase)(FLAT_VECTOR_TYPE(SUFFIX) *, size_t, size_t);                     \
    void (*clear)(FLAT_VECTOR_TYPE(SUFFIX) *);                                 \
    /* Container aliases */                                                    \
    OPSTATUS (*push)(FLAT_VECTOR_TYPE(SUFFIX) *, TYPE);                        \
    OPSTATUS (*pop)(FLAT_VECTOR_TYPE(SUFFIX) *, TYPE *);                       \
    OPSTATUS (*pushFront)(FLAT_VECTOR_TYPE(SUFFIX) *, TYPE);                  \
    OPSTATUS (*popFront)(FLAT_VECTOR_TYPE(SUFFIX) *, TYPE *);                 \
    /* Observers */                                                            \
    bool (*empty)(const FLAT_VECTOR_TYPE(SUFFIX) *);                                 \
    size_t (*size)(const FLAT_VECTOR_TYPE(SUFFIX) *);                                \
    size_t (*capacity)(const FLAT_VECTOR_TYPE(SUFFIX) *);                            \
  } FLAT_VECTOR_FUNC(SUFFIX, FunctionTable);

#define FLAT_VECTOR_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)                    \
  static const FLAT_VECTOR_FUNC(SUFFIX, FunctionTable)                             \
      FLAT_VECTOR_FUNC(SUFFIX, functions) = {                                  \
          .init = FLAT_VECTOR_FUNC(SUFFIX, Init),                      \
          .destroy = FLAT_VECTOR_FUNC(SUFFIX, Destroy),                \
          .reserve = FLAT_VECTOR_FUNC(SUFFIX, Reserve),                \
          .shrinkToFit = FLAT_VECTOR_FUNC(SUFFIX, ShrinkToFit),    \
          .at = FLAT_VECTOR_FUNC(SUFFIX, At),                          \
          .front = FLAT_VECTOR_FUNC(SUFFIX, Front),                    \
          .back = FLAT_VECTOR_FUNC(SUFFIX, Back),                      \
          .data = FLAT_VECTOR_FUNC(SUFFIX, Data),                      \
          .pushBack = FLAT_VECTOR_FUNC(SUFFIX, PushBack),            \
          .popBack = FLAT_VECTOR_FUNC(SUFFIX, PopBack),              \
          .insert = FLAT_VECTOR_FUNC(SUFFIX, Insert),                  \
          .erase = FLAT_VECTOR_FUNC(SUFFIX, Erase),                    \
          .clear = FLAT_VECTOR_FUNC(SUFFIX, Clear),                    \
          .push = FLAT_VECTOR_FUNC(SUFFIX, PushBack),                 \
          .pop = FLAT_VECTOR_FUNC(SUFFIX, PopBack),                   \
          .pushFront = FLAT_VECTOR_FUNC(SUFFIX, PushFront),          \
          .popFront = FLAT_VECTOR_FUNC(SUFFIX, PopFront),            \
          .empty = FLAT_VECTOR_FUNC(SUFFIX, Empty),                    \
          .size = FLAT_VECTOR_FUNC(SUFFIX, Size),                      \
          .capacity = FLAT_VECTOR_FUNC(SUFFIX, Capacity)};

#else
#define FLAT_VECTOR_FUNCTION_TABLE(...)
#define FLAT_VECTOR_FUNCTION_TABLE_INSTANCE(...)
#endif
