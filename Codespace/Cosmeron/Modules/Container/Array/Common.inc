#ifndef COSMERON_CONTAINER_ARRAY_COMMON_INC
#define COSMERON_CONTAINER_ARRAY_COMMON_INC

#if defined(CONTAINER_DISABLE_FUNCTION_TABLE) &&                               \
    !defined(OBJECT_DISABLE_FUNCTION_TABLE)
#define OBJECT_DISABLE_FUNCTION_TABLE 1
#endif

#if defined(ARRAY_DISABLE_FUNCTION_TABLE) &&                                   \
    !defined(OBJECT_DISABLE_FUNCTION_TABLE)
#define OBJECT_DISABLE_FUNCTION_TABLE 1
#endif

#include "../../../Core/Algorithm/Comparison.h"
#include "../../../Core/Memory/Alloc.h"
#include "../../../Core/Module/Module.h"
#include "../../../Core/Outcome/Status.h"
#include "../../../Core/Preprocessor/Compiling.inc"
#include "../../../Core/Preprocessor/Operations.h"
#include "../../../Core/Primitive/Types.h"
#include "../Iterator/Iterator.h"

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* ==========================================================================
 * Module Template Forwarding Helper
 * Enables: TEMPLATE_INSTANTIATE(Array, Target, Params...)
 * ========================================================================== */

#ifndef TEMPLATE_Array
#define TEMPLATE_Array(TARGET, ...) NS(TEMPLATE_Array, TARGET)(__VA_ARGS__)
#endif

/* ==========================================================================
 * Array Configuration Constants Table (Row-Tuple Pattern)
 * ========================================================================== */

#define ARRAY_CONFIG_COL_NAME(NAME, VALUE) NAME
#define ARRAY_CONFIG_COL_VALUE(NAME, VALUE) VALUE
#define ARRAY_CONFIG_GET(FIELD, ROW) PP_OP_CAT2(ARRAY_CONFIG_COL_, FIELD) ROW

#define ARRAY_CONFIG_TABLE(X)                                                  \
  X((INITIAL_CAPACITY, 8U))                                                    \
  X((GROWTH_FACTOR, 2U))

/* ==========================================================================
 * Shared Internal Capacity & Buffer Helpers (Zero FunctionTable Overhead)
 * ========================================================================== */

MODULE(DOMAIN(Container), Array,
  ENUM_EX(Config, ARRAY_CONFIG_TABLE, QUALIFIED),

  INTERNAL(
    OPERATION(ReserveBuffer,
              (void **inoutData, size_t *inoutCapacity, size_t minCapacity,
               size_t elementSize), {
      if (!inoutData || !inoutCapacity || elementSize == 0U)
        return STATUS_CONST(INVALID_ARGUMENT);
      if (minCapacity <= *inoutCapacity)
        return STATUS_CONST(SUCCESS);
      void *newData = *inoutData;
      OPSTATUS status = MODULE_FUNC(Memory, Alloc, ReallocArray)(
          &newData, minCapacity, elementSize);
      if (status != STATUS_CONST(SUCCESS))
        return status;
      *inoutData = newData;
      *inoutCapacity = minCapacity;
      return STATUS_CONST(SUCCESS);
    }),

    OPERATION(EnsureCapacity,
              (void **inoutData, size_t size, size_t *inoutCapacity,
               size_t elementSize), {
      if (!inoutData || !inoutCapacity || elementSize == 0U)
        return STATUS_CONST(INVALID_ARGUMENT);
      if (size < *inoutCapacity)
        return STATUS_CONST(SUCCESS);
      const size_t initialCap =
          (size_t)MODULE_CONST(Array, Config, INITIAL_CAPACITY);
      const size_t growthFactor =
          (size_t)MODULE_CONST(Array, Config, GROWTH_FACTOR);
      if (*inoutCapacity != 0U && *inoutCapacity > SIZE_MAX / growthFactor)
        return STATUS_CONST(ARITHMETIC_OVERFLOW);
      const size_t nextCapacity =
          (*inoutCapacity == 0U) ? initialCap : (*inoutCapacity * growthFactor);
      return MODULE_IFUNC(Array, ReserveBuffer)(
          inoutData, inoutCapacity, nextCapacity, elementSize);
    }),

    OPERATION(ShrinkBuffer,
              (void **inoutData, size_t size, size_t *inoutCapacity,
               size_t elementSize), {
      if (!inoutData || !inoutCapacity || elementSize == 0U)
        return STATUS_CONST(INVALID_ARGUMENT);
      if (size == *inoutCapacity)
        return STATUS_CONST(SUCCESS);
      if (size == 0U) {
        if (*inoutData) {
          MODULE_FUNC(Memory, Alloc, Free)(inoutData);
          *inoutData = NULL;
        }
        *inoutCapacity = 0U;
        return STATUS_CONST(SUCCESS);
      }
      void *newData = *inoutData;
      OPSTATUS status = MODULE_FUNC(Memory, Alloc, ReallocArray)(
          &newData, size, elementSize);
      if (status != STATUS_CONST(SUCCESS))
        return status;
      *inoutData = newData;
      *inoutCapacity = size;
      return STATUS_CONST(SUCCESS);
    })
  )
);

#endif /* COSMERON_CONTAINER_ARRAY_COMMON_INC */
