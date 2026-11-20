#ifndef COSMERON_CONTAINER_GRAPH_COMMON_INC
#define COSMERON_CONTAINER_GRAPH_COMMON_INC

#if (defined(CONTAINER_DISABLE_FUNCTION_TABLE) ||                               \
     defined(GRAPH_DISABLE_FUNCTION_TABLE)) &&                                 \
    !defined(COSMERON_DISABLE_FT_Container_Graph)
#define COSMERON_DISABLE_FT_Container_Graph 1
#endif
#if (defined(CONTAINER_DISABLE_FUNCTION_TABLE) ||                               \
     defined(GRAPH_DISABLE_FUNCTION_TABLE)) &&                                 \
    !defined(COSMERON_DISABLE_FT_Graph)
#define COSMERON_DISABLE_FT_Graph 1
#endif

#include "../../../../Core/Memory/Allocator.h"
#include "../../../../Core/Module/Module.h"
#include "../../../../Core/Condition/Status.h"
#include "../../../../Core/Preprocessor/Compiling.inc"
#include "../../../../Core/Preprocessor/Operations.h"
#include "../../../../Core/Types/Primitive.h"
#include "../../Impl/Common.inc"

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
                if (newCount > SIZE_MAX / elemSize)
                  return STATUS_CONST(OUT_OF_RANGE);
                return MODULE_FUNC(Memory, Allocator, Realloc)(NULL, *inoutPtr, 0U, newCount * elemSize, inoutPtr);
              }),

    PROCEDURE(FreeBuffer, (void **inoutPtr), {
      if (inoutPtr && *inoutPtr) {
        MODULE_FUNC(Memory, Allocator, Free)(NULL, *inoutPtr, 0U);
      }
    }),

    OPERATION(AllocTraversalBuffers,
              (size_t vertexCount, bool **outVisited, size_t **outWork), {
      if (!outVisited || !outWork || vertexCount == 0U)
        return STATUS_CONST(INVALID_ARGUMENT);
      void *visitedRaw = NULL;
      void *workRaw = NULL;
      OPSTATUS status = MODULE_IFUNC(Graph, AllocBuffer)(
          &visitedRaw, vertexCount, sizeof(bool));
      if (status != STATUS_CONST(SUCCESS))
        return status;
      status = MODULE_IFUNC(Graph, AllocBuffer)(
          &workRaw, vertexCount, sizeof(size_t));
      if (status != STATUS_CONST(SUCCESS)) {
        MODULE_IFUNC(Graph, FreeBuffer)(&visitedRaw);
        return status;
      }
      bool *visited = (bool *)visitedRaw;
      for (size_t i = 0U; i < vertexCount; i++)
        visited[i] = false;
      *outVisited = visited;
      *outWork = (size_t *)workRaw;
      return STATUS_CONST(SUCCESS);
    }),

    PROCEDURE(FreeTraversalBuffers, (bool *visited, size_t *work), {
      void *visitedRaw = (void *)visited;
      void *workRaw = (void *)work;
      MODULE_IFUNC(Graph, FreeBuffer)(&workRaw);
      MODULE_IFUNC(Graph, FreeBuffer)(&visitedRaw);
    })
  )
);

#endif /* COSMERON_CONTAINER_GRAPH_COMMON_INC */

