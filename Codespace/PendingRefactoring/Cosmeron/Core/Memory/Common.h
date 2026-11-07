#pragma once

#include "../Outcome/Status.h"
#include "../Module/Namespace.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef MEMORY_NS
#define MEMORY_NS(NAME) NS(LIB_PREFIX(Memory), NAME)
#endif
#ifndef MEMORY_CNS
#define MEMORY_CNS(NAME) CNS(LIB_PREFIX_CONST(MEMORY), NAME)
#endif
#ifndef MEMORY_INS
#define MEMORY_INS(NAME) NS(LIB_PREFIX(Memory), Internal, NAME)
#endif
#ifndef MEMORY_CINS
#define MEMORY_CINS(NAME) CNS(LIB_PREFIX_CONST(MEMORY), INTERNAL, NAME)
#endif

#ifndef MEMORY_TYPE
#define MEMORY_TYPE(SUFFIX) PP_OP_CAT3(T, Memory, SUFFIX)
#endif
#ifndef MEMORY_FUNC
#define MEMORY_FUNC(NAME) MEMORY_NS(NAME)
#endif
#ifndef MEMORY_CONST
#define MEMORY_CONST(NAME) MEMORY_CNS(NAME)
#endif

typedef union MEMORY_INS(MaxAlignment_InternalType_) {
  long double longDoubleValue;
  double doubleValue;
  long long integerValue;
  void *pointerValue;
} MEMORY_INS(MaxAlignment_InternalType_);

#define COSMERON_MACRO_INTERNAL_MEMORY_MAX_ALIGNMENT                           \
  _Alignof(MEMORY_INS(MaxAlignment_InternalType_))

static inline size_t MEMORY_INS(MaxAlignment)(void) {
  return _Alignof(MEMORY_INS(MaxAlignment_InternalType_));
}

static inline OPSTATUS MEMORY_INS(CheckedMultiplySize)(size_t count,
                                                      size_t elementSize,
                                                      size_t *outSize) {
  if (!outSize)
    return STATUS_CONST(INVALID_ARGUMENT);

  if (elementSize != 0U && count > SIZE_MAX / elementSize)
    return STATUS_CONST(ARITHMETIC_OVERFLOW);

  *outSize = count * elementSize;
  return STATUS_CONST(SUCCESS);
}

static inline void MEMORY_INS(WritePointer)(void *outPointerObject, void *value) {
  memcpy(outPointerObject, &value, sizeof(void *));
}

static inline void *MEMORY_INS(ReadPointer)(const void *pointerObject) {
  void *value = NULL;
  memcpy(&value, pointerObject, sizeof(void *));
  return value;
}

/* EOF */

