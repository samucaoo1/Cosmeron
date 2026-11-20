#pragma once

#include "../Condition/Status.h"
#include "../Module/Module.h"
#include "../Preprocessor/Compiling.inc"
#include "Impl/Common.inc"

/* ========================================================================== */
/* Package Declaration: Memory_Operations                                     */
/* ========================================================================== */

#if defined(MEMORY_DISABLE_FUNCTION_TABLE) &&                                  \
    !defined(COSMERON_DISABLE_FT_Memory)
#define COSMERON_DISABLE_FT_Memory 1
#endif
#if defined(MEMORY_OPERATIONS_DISABLE_FUNCTION_TABLE) &&                       \
    !defined(COSMERON_DISABLE_FT_Memory_Operations)
#define COSMERON_DISABLE_FT_Memory_Operations 1
#endif

MODULE(DOMAIN(Core), Memory,
    PACKAGE(Operations,
        VARIANT(Copy,
            OPERATION((void *destination, const void *source, size_t size)),
            OPERATION(Array, (void *destination, const void *source, size_t count, size_t elementSize))
        ),
        VARIANT(Move,
            OPERATION((void *destination, const void *source, size_t size)),
            OPERATION(Array, (void *destination, const void *source, size_t count, size_t elementSize))
        ),
        OPERATION(Set, (void *destination, int value, size_t size)),
        VARIANT(Zero,
            OPERATION((void *destination, size_t size)),
            OPERATION(Array, (void *destination, size_t count, size_t elementSize))
        ),
        PREDICATE(Equals, (const void *left, const void *right, size_t size))
    )
);

#include "Impl/Operations.impl"

/* EOF */
