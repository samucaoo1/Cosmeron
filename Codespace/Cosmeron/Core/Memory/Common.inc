#pragma once

#include "../Outcome/Status.h"
#include "../Module/Module.h"

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

MODULE(Memory,
  INTERNAL(
    FUNCTION(size_t, MaxAlignment, (void), {
      return _Alignof(MODULE_ITYPE(Memory, MaxAlignment_InternalType_));
    }),

    OPERATION(CheckedMultiplySize, (size_t count, size_t elementSize, size_t *outSize), {
      if (!outSize)
        return STATUS_CONST(INVALID_ARGUMENT);

      if (elementSize != 0U && count > SIZE_MAX / elementSize)
        return STATUS_CONST(ARITHMETIC_OVERFLOW);

      *outSize = count * elementSize;
      return STATUS_CONST(SUCCESS);
    }),

    PROCEDURE(WritePointer, (void *outPointerObject, void *value), {
      memcpy(outPointerObject, &value, sizeof(void *));
    }),

    FUNCTION(void *, ReadPointer, (const void *pointerObject), {
      void *value = NULL;
      memcpy(&value, pointerObject, sizeof(void *));
      return value;
    })
  )
);

/* EOF */
