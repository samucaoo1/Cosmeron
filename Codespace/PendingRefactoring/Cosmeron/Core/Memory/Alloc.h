#pragma once

#include "../Module/Package.h"
#include "../Preprocessor/Compiling.inc"
#include "Common.h"

#define ALLOC_NS(NAME) NS(LIB_PREFIX(Memory_Alloc), NAME)
#define ALLOC_CNS(NAME) CNS(LIB_PREFIX_CONST(MEMORY), ALLOC, NAME)
#define ALLOC_INS(NAME) NS(LIB_PREFIX(Memory_Alloc), Internal, NAME)
#define ALLOC_CINS(NAME) CNS(LIB_PREFIX_CONST(MEMORY), ALLOC, INTERNAL, NAME)

#define ALLOC_TYPE(NAME) ALLOC_NS(NAME)
#define ALLOC_FUNC(NAME) ALLOC_NS(NAME)
#define ALLOC_CONST(NAME) ALLOC_CNS(NAME)

PACKAGE(LIB_PREFIX(Memory_Alloc),
    OPERATION(Bytes, (void *outPointerObject, size_t size)),
    OPERATION(ZeroBytes, (void *outPointerObject, size_t size)),
    OPERATION(Array, (void *outPointerObject, size_t count, size_t elementSize)),
    OPERATION(ZeroArray, (void *outPointerObject, size_t count, size_t elementSize)),
    OPERATION(ReallocBytes, (void *inoutPointerObject, size_t newSize)),
    OPERATION(ReallocArray, (void *inoutPointerObject, size_t count, size_t elementSize)),
    OPERATION(Free, (void *inoutPointerObject))
);

static inline OPSTATUS ALLOC_FUNC(AllocBytes)(void *outPointerObject, size_t size) {
  return ALLOC_FUNC(Bytes)(outPointerObject, size);
}

static inline OPSTATUS ALLOC_FUNC(AllocArray)(void *outPointerObject, size_t count, size_t elementSize) {
  return ALLOC_FUNC(Array)(outPointerObject, count, elementSize);
}

static inline OPSTATUS ALLOC_FUNC(FreePointer)(void *inoutPointerObject) {
  return ALLOC_FUNC(Free)(inoutPointerObject);
}

#define Memory_Free(pointerLvalue) ALLOC_FUNC(Free)(&(pointerLvalue))

#include "Impl/Alloc.impl"
/* EOF */
