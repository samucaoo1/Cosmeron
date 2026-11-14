#ifndef COSMERON_CONTAINER_GRAPH_COMMON_INC
#define COSMERON_CONTAINER_GRAPH_COMMON_INC

#if defined(CONTAINER_DISABLE_FUNCTION_TABLE) &&                               \
    !defined(OBJECT_DISABLE_FUNCTION_TABLE)
#define OBJECT_DISABLE_FUNCTION_TABLE 1
#endif

#if defined(GRAPH_DISABLE_FUNCTION_TABLE) &&                                   \
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

/* ==========================================================================
 * Module Template Forwarding Helper
 * Enables: TEMPLATE_INSTANTIATE(Graph, Target, Params...)
 * ========================================================================== */

#ifndef TEMPLATE_Graph
#define TEMPLATE_Graph(TARGET, ...) NS(TEMPLATE_Graph, TARGET)(__VA_ARGS__)
#endif

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
      return MODULE_FUNC(Memory, Alloc, Bytes)(outPtr, count * elemSize);
    }),

    OPERATION(ReallocBuffer,
              (void **inoutPtr, size_t newCount, size_t elemSize), {
                if (!inoutPtr || newCount == 0U || elemSize == 0U)
                  return STATUS_CONST(INVALID_ARGUMENT);
                return MODULE_FUNC(Memory, Alloc, ReallocArray)(
                    inoutPtr, newCount, elemSize);
              }),

    PROCEDURE(FreeBuffer, (void **inoutPtr), {
      if (inoutPtr && *inoutPtr) {
        MODULE_FUNC(Memory, Alloc, Free)(inoutPtr);
      }
    })
  )
);

#endif /* COSMERON_CONTAINER_GRAPH_COMMON_INC */

