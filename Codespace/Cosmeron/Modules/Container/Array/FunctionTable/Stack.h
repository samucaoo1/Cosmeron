#pragma once

#if CONTAINER_FUNCTION_TABLE_ENABLED

#define FLAT_STACK_FUNCTION_TABLE_STRUCT(TYPE, SUFFIX)                              \
  typedef struct FLAT_STACK_FUNC(SUFFIX, FunctionTable) {                   \
    /* Lifecycle */                                                            \
    OPSTATUS (*init)(FLAT_STACK_TYPE(SUFFIX) *);                               \
    void (*destroy)(FLAT_STACK_TYPE(SUFFIX) *);                                \
    /* Capacity */                                                             \
    OPSTATUS (*reserve)(FLAT_STACK_TYPE(SUFFIX) *, size_t);                    \
    OPSTATUS (*shrinkToFit)(FLAT_STACK_TYPE(SUFFIX) *);                      \
    /* Modifiers / Container aliases */                                        \
    OPSTATUS (*push)(FLAT_STACK_TYPE(SUFFIX) *, TYPE);                         \
    OPSTATUS (*pop)(FLAT_STACK_TYPE(SUFFIX) *, TYPE *);                        \
    /* Element access */                                                       \
    OPSTATUS (*top)(FLAT_STACK_TYPE(SUFFIX) *, TYPE **);                                   \
    /* Observers */                                                            \
    bool (*empty)(const FLAT_STACK_TYPE(SUFFIX) *);                                  \
    size_t (*size)(const FLAT_STACK_TYPE(SUFFIX) *);                                 \
    size_t (*capacity)(const FLAT_STACK_TYPE(SUFFIX) *);                             \
  } FLAT_STACK_FUNC(SUFFIX, FunctionTable);

#define FLAT_STACK_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)                     \
  static const FLAT_STACK_FUNC(SUFFIX, FunctionTable)                              \
      FLAT_STACK_FUNC(SUFFIX, functions) = {                                   \
          .init = FLAT_STACK_FUNC(SUFFIX, Init),                       \
          .destroy = FLAT_STACK_FUNC(SUFFIX, Destroy),                 \
          .reserve = FLAT_STACK_FUNC(SUFFIX, Reserve),                 \
          .shrinkToFit = FLAT_STACK_FUNC(SUFFIX, ShrinkToFit),     \
          .push = FLAT_STACK_FUNC(SUFFIX, Push),                       \
          .pop = FLAT_STACK_FUNC(SUFFIX, Pop),                         \
          .top = FLAT_STACK_FUNC(SUFFIX, Top),                         \
          .empty = FLAT_STACK_FUNC(SUFFIX, Empty),                     \
          .size = FLAT_STACK_FUNC(SUFFIX, Size),                       \
          .capacity = FLAT_STACK_FUNC(SUFFIX, Capacity)};

#else
#define FLAT_STACK_FUNCTION_TABLE_STRUCT(...)
#define FLAT_STACK_FUNCTION_TABLE_INSTANCE(...)
#endif
