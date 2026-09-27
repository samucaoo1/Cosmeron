#pragma once

#if CONTAINER_ENABLE_PSEUDO_METHODS

#define LINKED_STACK_FUNCTION_TABLE(TYPE, SUFFIX)                           \
  typedef struct LINKED_STACK_FUNC(SUFFIX, FunctionTable) {               \
    /* Lifecycle */                                                            \
    OPSTATUS (*init)(LINKED_STACK_TYPE(SUFFIX) *);                            \
    void (*destroy)(LINKED_STACK_TYPE(SUFFIX) *);                             \
    /* Modifiers / Container aliases */                                        \
    OPSTATUS (*push)(LINKED_STACK_TYPE(SUFFIX) *, TYPE);                      \
    OPSTATUS (*pop)(LINKED_STACK_TYPE(SUFFIX) *, TYPE *);                     \
    void (*clear)(LINKED_STACK_TYPE(SUFFIX) *);                               \
    /* Element access */                                                       \
    TYPE *(*top)(LINKED_STACK_TYPE(SUFFIX) *);                                \
    /* Observers */                                                            \
    bool (*empty)(const LINKED_STACK_TYPE(SUFFIX) *);                               \
    size_t (*size)(const LINKED_STACK_TYPE(SUFFIX) *);                              \
  } LINKED_STACK_FUNC(SUFFIX, FunctionTable);

#define LINKED_STACK_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)                  \
  static const LINKED_STACK_FUNC(SUFFIX, FunctionTable)                           \
      LINKED_STACK_FUNC(SUFFIX, functions) = {                                \
          /* Lifecycle */                                                       \
          .init = LINKED_STACK_FUNC(SUFFIX, Init),                    \
          .destroy = LINKED_STACK_FUNC(SUFFIX, Destroy),              \
          /* Modifiers / Container aliases */                                   \
          .push = LINKED_STACK_FUNC(SUFFIX, Push),                    \
          .pop = LINKED_STACK_FUNC(SUFFIX, Pop),                      \
          .clear = LINKED_STACK_FUNC(SUFFIX, Clear),                  \
          /* Element access */                                                  \
          .top = LINKED_STACK_FUNC(SUFFIX, Top),                      \
          /* Observers */                                                       \
          .empty = LINKED_STACK_FUNC(SUFFIX, Empty),                  \
          .size = LINKED_STACK_FUNC(SUFFIX, Size)};

#else
#define LINKED_STACK_FUNCTION_TABLE(...)
#define LINKED_STACK_FUNCTION_TABLE_INSTANCE(...)
#endif
