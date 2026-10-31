#pragma once

#include "../Module/Package.h"
#include "../Preprocessor/Compiling.inc"
#include "Memory.space"

PACKAGE(LIB_PREFIX(MEMORY_MOD),
    OPERATION(Copy, (void *destination, const void *source, size_t size)),
    OPERATION(Move, (void *destination, const void *source, size_t size)),
    OPERATION(Set, (void *destination, int value, size_t size)),
    OPERATION(Zero, (void *destination, size_t size)),
    OPERATION(CopyArray, (void *destination, const void *source, size_t count, size_t elementSize)),
    OPERATION(MoveArray, (void *destination, const void *source, size_t count, size_t elementSize)),
    OPERATION(ZeroArray, (void *destination, size_t count, size_t elementSize)),
    PREDICATE(Equals, (const void *left, const void *right, size_t size))
);

#include "Impl/Operations.impl"
/* EOF */
