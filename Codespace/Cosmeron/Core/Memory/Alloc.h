#pragma once

#include "../Condition/Status.h"
#include "../Module/Module.h"
#include "../Preprocessor/Compiling.inc"
#include "Impl/Common.inc"

/* ========================================================================== */
/* Package Declaration: Memory_Alloc                                          */
/* ========================================================================== */

MODULE(DOMAIN(Core), Memory,
    PACKAGE(Alloc,
        OPERATION(Bytes, (void *outPointerObject, size_t size)),
        OPERATION(Array, (void *outPointerObject, size_t count, size_t elementSize)),
        VARIANT(Zero,
            OPERATION(Bytes, (void *outPointerObject, size_t size)),
            OPERATION(Array, (void *outPointerObject, size_t count, size_t elementSize))
        ),
        VARIANT(Realloc,
            OPERATION(Bytes, (void *inoutPointerObject, size_t newSize)),
            OPERATION(Array, (void *inoutPointerObject, size_t count, size_t elementSize))
        ),
        OPERATION(Free, (void *inoutPointerObject))
    )
);

#define Memory_Free(pointerLvalue) MODULE_FUNC(Memory, Alloc, Free)(&(pointerLvalue))

#include "Impl/Alloc.impl"

/* EOF */
