#pragma once

#include "../Object.space"

#ifndef OBJECT_DISABLE_FUNCTION_TABLE

typedef struct OBJECT_TYPE(TObject_FunctionTable) {
  OPSTATUS (*destroy)(void *self);
  OPSTATUS (*toString)(const void *self, char *dest, size_t capacity);
  bool (*equals)(const void *self, const void *other);
  uint64_t (*hash)(const void *self);
} OBJECT_TYPE(TObject_FunctionTable);

#define OBJECT_FUNCTION_TABLE_INSTANCE                                         \
  static const OBJECT_TYPE(TObject_FunctionTable) OBJECT_FUNC(functions) = {   \
      .destroy = OBJECT_FUNC(Destroy),                                         \
      .toString = OBJECT_FUNC(ToString),                                       \
      .equals = OBJECT_FUNC(Equals),                                           \
      .hash = OBJECT_FUNC(Hash)};

#endif
/* EOF */
