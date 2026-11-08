#pragma once

#include "../Outcome/Status.h"
#include "../Module/Module.h"
#include "../Preprocessor/Compiling.inc"
#include "Common.inc"

/* ========================================================================== */
/* Package Declaration: Memory_Operations                                     */
/* ========================================================================== */

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
