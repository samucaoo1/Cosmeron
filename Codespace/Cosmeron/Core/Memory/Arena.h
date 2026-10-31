#pragma once

#include "../Module/Object.h"
#include "../Preprocessor/Compiling.inc"
#include "Alloc.h"
#include "Memory.space"

#define ARENA_MOD GNS2(MEMORY_MOD, Arena)
#define ARENA_CMOD PP_OP_CAT3(MEMORY_CMOD, _, ARENA)

#define ARENA_NS(NAME) GNS3(LIB_PREFIX(MEMORY_MOD), Arena, NAME)
#define ARENA_CNS(NAME) CNS3(LIB_PREFIX_CONST(MEMORY_CMOD), ARENA, NAME)
#define ARENA_INS(NAME) GNS4(LIB_PREFIX(MEMORY_MOD), Arena, Internal, NAME)
#define ARENA_CINS(NAME) CNS4(LIB_PREFIX_CONST(MEMORY_CMOD), ARENA, INTERNAL, NAME)

#define ARENA_TYPE(NAME) ARENA_NS(NAME)
#define ARENA_FUNC(NAME) GNS2(ARENA_TYPE(TArena), NAME)
#define ARENA_CONST(NAME) ARENA_CNS(NAME)

OBJECT(ARENA_TYPE(TArena),
    FIELDS(
        uint8_t *buffer;
        size_t capacity;
        size_t offset;
        bool isOwned;
    ),
    OPERATION(Create, (size_t capacity)),
    OPERATION(CreateBuffer, (void *buffer, size_t capacity)),
    OPERATION(AllocBytes, (void *outPointerObject, size_t size)),
    OPERATION(AllocAligned, (void *outPointerObject, size_t size, size_t alignment)),
    OPERATION(AllocArray, (void *outPointerObject, size_t count, size_t elementSize)),
    OPERATION(AllocArrayAligned, (void *outPointerObject, size_t count, size_t elementSize, size_t alignment)),
    CONST_OPERATION(Save, (size_t *outMark)),
    OPERATION(Restore, (size_t mark)),
    OPERATION(Reset, ()),
    OPERATION(Destroy, ()),
    CONST_FUNCTION_INLINE(size_t, Capacity, (), { return self ? self->capacity : 0U; }),
    CONST_FUNCTION_INLINE(size_t, Used, (), { return self ? self->offset : 0U; }),
    CONST_FUNCTION_INLINE(size_t, Remaining, (), {
      return (self && self->offset <= self->capacity) ? (self->capacity - self->offset) : 0U;
    }),
    CONST_PREDICATE(Contains, (const void *ptr))
);

typedef ARENA_TYPE(TArena) TArena;

#include "Impl/Arena.impl"
/* EOF */
