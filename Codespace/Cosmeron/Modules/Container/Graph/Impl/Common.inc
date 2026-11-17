#ifndef COSMERON_CONTAINER_GRAPH_COMMON_INC
#define COSMERON_CONTAINER_GRAPH_COMMON_INC

#if (defined(CONTAINER_DISABLE_FUNCTION_TABLE) ||                               \
     defined(GRAPH_DISABLE_FUNCTION_TABLE)) &&                                 \
    !defined(COSMERON_DISABLE_FT_Container_Graph)
#define COSMERON_DISABLE_FT_Container_Graph 1
#endif

#include "../../../../Core/Memory/Allocator.h"
#include "../../../../Core/Module/Module.h"
#include "../../../../Core/Condition/Status.h"
#include "../../../../Core/Preprocessor/Compiling.inc"
#include "../../../../Core/Preprocessor/Operations.h"
#include "../../../../Core/Types/Primitive.h"
#include "../../Iterator/Iterator.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ==========================================================================
 * Graph Config Table (Row-Tuple X-Macro Pattern)
 * ========================================================================== */

#define GRAPH_CONFIG_COL_NAME(NAME, VALUE) NAME
#define GRAPH_CONFIG_COL_VALUE(NAME, VALUE) VALUE
#define GRAPH_CONFIG_GET(FIELD, ROW) PP_OP_CAT2(GRAPH_CONFIG_COL_, FIELD) ROW

#define GRAPH_CONFIG_TABLE(X)                                                  \
  X((INITIAL_VERTEX_CAPACITY, 8U))                                             \
  X((INITIAL_EDGE_CAPACITY, 4U))                                               \
  X((GROWTH_FACTOR, 2U))

/* ==========================================================================
 * Shared Internal Buffer Helpers (Zero FunctionTable Overhead)
 * ========================================================================== */

MODULE(DOMAIN(Container), Graph,
  ENUM_EX(Config, GRAPH_CONFIG_TABLE, QUALIFIED),

  INTERNAL(
    OPERATION(AllocBuffer, (void **outPtr, size_t count, size_t elemSize), {
      if (!outPtr || count == 0U || elemSize == 0U)
        return STATUS_CONST(INVALID_ARGUMENT);
      if (count > SIZE_MAX / elemSize)
        return STATUS_CONST(OUT_OF_RANGE);
      return MODULE_FUNC(Memory, Allocator, Alloc)(NULL, count * elemSize, outPtr);
    }),

    OPERATION(ReallocBuffer,
              (void **inoutPtr, size_t newCount, size_t elemSize), {
                if (!inoutPtr || newCount == 0U || elemSize == 0U)
                  return STATUS_CONST(INVALID_ARGUMENT);
                return MODULE_FUNC(Memory, Allocator, Realloc)(NULL, *inoutPtr, 0U, newCount * elemSize, inoutPtr);
              }),

    PROCEDURE(FreeBuffer, (void **inoutPtr), {
      if (inoutPtr && *inoutPtr) {
        MODULE_FUNC(Memory, Allocator, Free)(NULL, *inoutPtr, 0U);
      }
    })
  )
);

#endif /* COSMERON_CONTAINER_GRAPH_COMMON_INC */

