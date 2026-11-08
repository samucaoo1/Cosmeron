#pragma once

#include "../Outcome/Status.h"
#include "../Module/Module.h"
#include "../Preprocessor/Compiling.inc"
#include "Common.inc"

/* ========================================================================== */
/* Package Declaration: Memory_Alloc                                          */
/* ========================================================================== */

MODULE(DOMAIN(Core), Memory,
    PACKAGE(Alloc,
        OPERATION(Bytes, (void *outPointerObject, size_t size)),
        OPERATION(ZeroBytes, (void *outPointerObject, size_t size)),
        OPERATION(Array, (void *outPointerObject, size_t count, size_t elementSize)),
        OPERATION(ZeroArray, (void *outPointerObject, size_t count, size_t elementSize)),
        OPERATION(ReallocBytes, (void *inoutPointerObject, size_t newSize)),
        OPERATION(ReallocArray, (void *inoutPointerObject, size_t count, size_t elementSize)),
        OPERATION(Free, (void *inoutPointerObject))
    )
);

#define Memory_Free(pointerLvalue) MODULE_FUNC(Memory, Alloc, Free)(&(pointerLvalue))

#include "Impl/Alloc.impl"

/* EOF */

