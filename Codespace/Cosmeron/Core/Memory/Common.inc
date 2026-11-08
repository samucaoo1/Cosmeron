#pragma once

#include "../Outcome/Status.h"
#include "../Module/Namespace.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* ========================================================================== */
/* Internal Shared Infrastructure for Memory Module                           */
/* ========================================================================== */

typedef union MODULE_ITYPE(Memory, MaxAlignment_InternalType_) {
  long double longDoubleValue;
  double doubleValue;
  long long integerValue;
  void *pointerValue;
} MODULE_ITYPE(Memory, MaxAlignment_InternalType_);

#define COSMERON_MACRO_INTERNAL_MEMORY_MAX_ALIGNMENT                           \
  _Alignof(MODULE_ITYPE(Memory, MaxAlignment_InternalType_))

static inline size_t MODULE_IFUNC(Memory, MaxAlignment)(void) {
  return _Alignof(MODULE_ITYPE(Memory, MaxAlignment_InternalType_));
}

static inline OPSTATUS MODULE_IFUNC(Memory, CheckedMultiplySize)(
    size_t count, size_t elementSize, size_t *outSize) {
  if (!outSize)
    return STATUS_CONST(INVALID_ARGUMENT);

  if (elementSize != 0U && count > SIZE_MAX / elementSize)
    return STATUS_CONST(ARITHMETIC_OVERFLOW);

  *outSize = count * elementSize;
  return STATUS_CONST(SUCCESS);
}

static inline void MODULE_IFUNC(Memory, WritePointer)(
    void *outPointerObject, void *value) {
  memcpy(outPointerObject, &value, sizeof(void *));
}

static inline void *MODULE_IFUNC(Memory, ReadPointer)(
    const void *pointerObject) {
  void *value = NULL;
  memcpy(&value, pointerObject, sizeof(void *));
  return value;
}

/* EOF */
