#ifndef COSMERON_CONTAINER_HASH_COMMON_INC
#define COSMERON_CONTAINER_HASH_COMMON_INC

#if defined(CONTAINER_DISABLE_FUNCTION_TABLE) &&                               \
    !defined(OBJECT_DISABLE_FUNCTION_TABLE)
#define OBJECT_DISABLE_FUNCTION_TABLE 1
#endif

#if defined(HASH_DISABLE_FUNCTION_TABLE) &&                                    \
    !defined(OBJECT_DISABLE_FUNCTION_TABLE)
#define OBJECT_DISABLE_FUNCTION_TABLE 1
#endif

#include "../../../Core/Memory/Alloc.h"
#include "../../../Core/Module/Module.h"
#include "../../../Core/Outcome/Status.h"
#include "../../../Core/Preprocessor/Compiling.inc"
#include "../../../Core/Preprocessor/Operations.h"
#include "../../../Core/Primitive/Types.h"
#include "../Iterator/Iterator.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* ==========================================================================
 * Module Template Forwarding Helper
 * Enables: TEMPLATE_INSTANTIATE(Hash, Target, Params...)
 * ========================================================================== */

#ifndef TEMPLATE_Hash
#define TEMPLATE_Hash(TARGET, ...) NS(TEMPLATE_Hash, TARGET)(__VA_ARGS__)
#endif

/* ==========================================================================
 * Hash Enums & Config Tables (Row-Tuple X-Macro Pattern)
 * ========================================================================== */

#define HASH_BUCKET_STATE_COL_NAME(NAME, VALUE) NAME
#define HASH_BUCKET_STATE_COL_VALUE(NAME, VALUE) VALUE
#define HASH_BUCKET_STATE_GET(FIELD, ROW)                                      \
  PP_OP_CAT2(HASH_BUCKET_STATE_COL_, FIELD) ROW

#define HASH_BUCKET_STATE_TABLE(X)                                             \
  X((EMPTY, 0))                                                                \
  X((OCCUPIED, 1))                                                             \
  X((DELETED, 2))

#define HASH_CONFIG_COL_NAME(NAME, VALUE) NAME
#define HASH_CONFIG_COL_VALUE(NAME, VALUE) VALUE
#define HASH_CONFIG_GET(FIELD, ROW) PP_OP_CAT2(HASH_CONFIG_COL_, FIELD) ROW

#define HASH_CONFIG_TABLE(X)                                                   \
  X((INITIAL_CAPACITY, 16U))                                                   \
  X((LOAD_PERCENT, 75U))                                                       \
  X((GROWTH_FACTOR, 2U))                                                       \
  X((PERCENT_BASE, 100U))

/* ==========================================================================
 * Shared Internal Bucket & Hashing Helpers (Zero FunctionTable Overhead)
 * ========================================================================== */

MODULE(DOMAIN(Container), Hash,
  ENUM_EX(BucketState, HASH_BUCKET_STATE_TABLE, QUALIFIED),
  ENUM_EX(Config, HASH_CONFIG_TABLE, QUALIFIED),

  INTERNAL(
    OPERATION(AllocBuckets, (void **outBuckets, size_t count, size_t elemSize), {
      if (!outBuckets || count == 0U || elemSize == 0U)
        return STATUS_CONST(INVALID_ARGUMENT);
      if (count > SIZE_MAX / elemSize)
        return STATUS_CONST(OUT_OF_RANGE);
      return MODULE_FUNC(Memory, Alloc, Bytes)(outBuckets, count * elemSize);
    }),

    PROCEDURE(FreeBuckets, (void **inoutBuckets), {
      if (inoutBuckets && *inoutBuckets) {
        MODULE_FUNC(Memory, Alloc, Free)(inoutBuckets);
      }
    }),

    FUNCTION(size_t, HashBytes, (const void *data, size_t length), {
      static const uint64_t fnvOffsetBasis = 14695981039346656037ULL;
      static const uint64_t fnvPrime = 1099511628211ULL;
      const unsigned char *bytes = (const unsigned char *)data;
      uint64_t hash = fnvOffsetBasis;
      for (size_t i = 0U; i < length; i++) {
        hash ^= (uint64_t)bytes[i];
        hash *= fnvPrime;
      }
      return (size_t)hash;
    }),

    FUNCTION(size_t, NormalizeCapacity, (size_t requested), {
      size_t cap = Hash_Config_INITIAL_CAPACITY;
      while (cap < requested) {
        if (cap > SIZE_MAX / Hash_Config_GROWTH_FACTOR)
          return requested;
        cap *= Hash_Config_GROWTH_FACTOR;
      }
      return cap;
    })
  )
);

#endif /* COSMERON_CONTAINER_HASH_COMMON_INC */

