#pragma once
#ifndef SORT_DISABLE_FUNCTION_TABLE
#define SORT_FUNCTION_TABLE_STRUCT                                           \
  typedef struct SORT_TYPE(TFunctionTable) {                                 \
    OPSTATUS (*Array)(void *, size_t, size_t, COMPARISON_TYPE(Comparator));   \
    OPSTATUS (*Stable)(void *, size_t, size_t, COMPARISON_TYPE(Comparator));  \
    OPSTATUS (*Insertion)(void *, size_t, size_t, COMPARISON_TYPE(Comparator));\
    OPSTATUS (*Heap)(void *, size_t, size_t, COMPARISON_TYPE(Comparator));    \
    OPSTATUS (*Intro)(void *, size_t, size_t, COMPARISON_TYPE(Comparator));   \
    OPSTATUS (*Merge)(void *, size_t, size_t, COMPARISON_TYPE(Comparator));   \
    OPSTATUS (*Reverse)(void *, size_t, size_t);                              \
  } SORT_TYPE(TFunctionTable)
#define SORT_FUNCTION_TABLE_INSTANCE                                         \
  static const SORT_TYPE(TFunctionTable) SORT_FUNC(FunctionTable) = {        \
    SORT_FUNC(Array), SORT_FUNC(Stable), SORT_FUNC(Insertion),               \
    SORT_FUNC(Heap), SORT_FUNC(Intro), SORT_FUNC(Merge), SORT_FUNC(Reverse)   \
  }
SORT_FUNCTION_TABLE_STRUCT;
SORT_FUNCTION_TABLE_INSTANCE;
#endif
