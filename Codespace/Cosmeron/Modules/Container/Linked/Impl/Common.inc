#ifndef COSMERON_CONTAINER_LINKED_COMMON_INC
#define COSMERON_CONTAINER_LINKED_COMMON_INC

#if (defined(CONTAINER_DISABLE_FUNCTION_TABLE) ||                               \
     defined(LINKED_DISABLE_FUNCTION_TABLE)) &&                                \
    !defined(COSMERON_DISABLE_FT_Container_Linked)
#define COSMERON_DISABLE_FT_Container_Linked 1
#endif

#include "../../../../Core/Memory/Allocator.h"
#include "../../../../Core/Module/Module.h"
#include "../../../../Core/Condition/Status.h"
#include "../../../../Core/Preprocessor/Compiling.inc"
#include "../../../../Core/Preprocessor/Operations.h"
#include "../../../../Core/Types/Primitive.h"
#include "../../Iterator/Iterator.h"
#include "../TSingleNode.h"
#include "../TDoubleNode.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ==========================================================================
 * Module Template Forwarding Helper
 * Enables: TEMPLATE_INSTANTIATE(Linked, Target, Params...)
 * ========================================================================== */

#ifndef TEMPLATE_Linked
#define TEMPLATE_Linked(TARGET, ...) NS(TEMPLATE_Linked, TARGET)(__VA_ARGS__)
#endif

/* ==========================================================================
 * Shared Internal Node Allocation Helpers (Zero FunctionTable Overhead)
 * ========================================================================== */

MODULE(DOMAIN(Container), Linked,
  INTERNAL(
    OPERATION(AllocNode, (void **outNode, size_t nodeSize), {
      if (!outNode || nodeSize == 0U)
        return STATUS_CONST(INVALID_ARGUMENT);
      return MODULE_FUNC(Memory, Allocator, Alloc)(NULL, nodeSize, outNode);
    }),

    PROCEDURE(FreeNode, (void **nodePtr), {
      if (nodePtr && *nodePtr) {
        MODULE_FUNC(Memory, Allocator, Free)(NULL, *nodePtr, 0U);
      }
    })
  )
);

#endif /* COSMERON_CONTAINER_LINKED_COMMON_INC */

