#pragma once

#if COSMERON_MACRO_INTERNAL_CONTAINER_FUNCTION_TABLE_ENABLED

#define FLAT_HEAP_FUNCTION_TABLE_STRUCT(TYPE, SUFFIX)                          \
  typedef struct FLAT_HEAP_FUNCTION_TABLE_TYPE(SUFFIX) {                       \
    /* Lifecycle */                                                            \
    OPSTATUS (*init)(FLAT_HEAP_TYPE(SUFFIX) *);                                \
    OPSTATUS (*initMax)(FLAT_HEAP_TYPE(SUFFIX) *);                             \
    OPSTATUS (*initWithComparator)(FLAT_HEAP_TYPE(SUFFIX) *,                   \
                                   CMPOUT (*)(TYPE, TYPE));                    \
    void (*destroy)(FLAT_HEAP_TYPE(SUFFIX) *);                                 \
    void (*clear)(FLAT_HEAP_TYPE(SUFFIX) *);                                   \
    /* Capacity */                                                             \
    OPSTATUS (*reserve)(FLAT_HEAP_TYPE(SUFFIX) *, size_t);                     \
    OPSTATUS (*shrinkToFit)(FLAT_HEAP_TYPE(SUFFIX) *);                         \
    /* Modifiers */                                                            \
    OPSTATUS (*push)(FLAT_HEAP_TYPE(SUFFIX) *, TYPE);                          \
    OPSTATUS (*pop)(FLAT_HEAP_TYPE(SUFFIX) *, TYPE *);                         \
    OPSTATUS (*peek)(const FLAT_HEAP_TYPE(SUFFIX) *, TYPE *);                  \
    OPSTATUS (*top)(FLAT_HEAP_TYPE(SUFFIX) *, TYPE **);                        \
    OPSTATUS (*replace)(FLAT_HEAP_TYPE(SUFFIX) *, TYPE, TYPE *);               \
    OPSTATUS (*pushPop)(FLAT_HEAP_TYPE(SUFFIX) *, TYPE, TYPE *);               \
    OPSTATUS (*heapify)(FLAT_HEAP_TYPE(SUFFIX) *);                             \
    OPSTATUS (*fromBuffer)(FLAT_HEAP_TYPE(SUFFIX) *, const TYPE *, size_t);    \
    /* Observers */                                                            \
    bool (*empty)(const FLAT_HEAP_TYPE(SUFFIX) *);                             \
    size_t (*size)(const FLAT_HEAP_TYPE(SUFFIX) *);                            \
    size_t (*capacity)(const FLAT_HEAP_TYPE(SUFFIX) *);                        \
  } FLAT_HEAP_FUNCTION_TABLE_TYPE(SUFFIX);

#define FLAT_HEAP_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)                        \
  static const FLAT_HEAP_FUNCTION_TABLE_TYPE(SUFFIX)                           \
      FLAT_HEAP_FUNC(SUFFIX, functions) = {                                    \
          .init = FLAT_HEAP_FUNC(SUFFIX, Init),                                \
          .initMax = FLAT_HEAP_FUNC(SUFFIX, InitMax),                          \
          .initWithComparator = FLAT_HEAP_FUNC(SUFFIX, InitWithComparator),    \
          .destroy = FLAT_HEAP_FUNC(SUFFIX, Destroy),                          \
          .clear = FLAT_HEAP_FUNC(SUFFIX, Clear),                              \
          .reserve = FLAT_HEAP_FUNC(SUFFIX, Reserve),                          \
          .shrinkToFit = FLAT_HEAP_FUNC(SUFFIX, ShrinkToFit),                  \
          .push = FLAT_HEAP_FUNC(SUFFIX, Push),                                \
          .pop = FLAT_HEAP_FUNC(SUFFIX, Pop),                                  \
          .peek = FLAT_HEAP_FUNC(SUFFIX, Peek),                                \
          .top = FLAT_HEAP_FUNC(SUFFIX, Top),                                  \
          .replace = FLAT_HEAP_FUNC(SUFFIX, Replace),                          \
          .pushPop = FLAT_HEAP_FUNC(SUFFIX, PushPop),                          \
          .heapify = FLAT_HEAP_FUNC(SUFFIX, Heapify),                          \
          .fromBuffer = FLAT_HEAP_FUNC(SUFFIX, FromBuffer),                    \
          .empty = FLAT_HEAP_FUNC(SUFFIX, Empty),                              \
          .size = FLAT_HEAP_FUNC(SUFFIX, Size),                                \
          .capacity = FLAT_HEAP_FUNC(SUFFIX, Capacity)};

#else
#define FLAT_HEAP_FUNCTION_TABLE_STRUCT(...)
#define FLAT_HEAP_FUNCTION_TABLE_INSTANCE(...)
#endif
