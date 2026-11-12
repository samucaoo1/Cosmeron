#pragma once

#include "../Algorithm/Comparison.h"
#include "../Module/Module.h"
#include "../Outcome/Status.h"
#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Detect.h"
#include "../Preprocessor/Operations.h"
#include "Types.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* 1. Trait Configuration Table                                               */
/* ========================================================================== */

#define PRIMITIVE_TRAIT_CONFIG_COL_NAME(NAME, VALUE) NAME
#define PRIMITIVE_TRAIT_CONFIG_COL_VALUE(NAME, VALUE) VALUE
#define PRIMITIVE_TRAIT_CONFIG_GET(FIELD, ROW)                                 \
  PP_OP_CAT2(PRIMITIVE_TRAIT_CONFIG_COL_, FIELD) ROW

#define PRIMITIVE_TRAIT_CONFIG_TABLE(X)                                        \
  X((SWAP_BUFFER_SIZE, 64))


/* ========================================================================== */
/* 2. Declarative Module: Primitive (TraitFlag, Callbacks, TTraits, Package)  */
/* ========================================================================== */

#define PRIMITIVE_TRAITS_PRIMITIVE_GETTER_DECL(ROW)                            \
  FUNCTION(const MODULE_TYPE(Primitive, Traits) *,                             \
           PRIMITIVE_TYPE_GET(NAME, ROW), (void)),

MODULE(DOMAIN(Core), Primitive,

    /* 2.1 Trait Behavioral Flags Enum */
    ENUM_EX(TraitFlag, PRIMITIVE_TRAIT_FLAG_TABLE, QUALIFIED),

    /* 2.2 Trait Lifecycle & Operation Callbacks */
    CALLBACK(
        OPERATION(TraitInit, (void *instance)),
        PROCEDURE(TraitDispose, (void *instance)),
        OPERATION(TraitCopy, (void *destination, const void *source)),
        PROCEDURE(TraitMove, (void *destination, void *source)),
        COMPARISON(TraitCompare, (const void *left, const void *right)),
        FUNCTION(size_t, TraitHash, (const void *instance))
    ),

    /* 2.3 Canonical Pure POD Static Trait Descriptor (TTraits) */
    STRUCT(Traits,
        FIELDS(
            const char *name;
            size_t size;
            size_t alignment;
            MODULE_ENUM(Primitive, Category) category;
            uint32_t flags;
            MODULE_CALLBACK(Primitive, TraitInit) init;
            MODULE_CALLBACK(Primitive, TraitDispose) dispose;
            MODULE_CALLBACK(Primitive, TraitCopy) copy;
            MODULE_CALLBACK(Primitive, TraitMove) move;
            MODULE_CALLBACK(Primitive, TraitCompare) compare;
            MODULE_CALLBACK(Primitive, TraitHash) hash;
        )
    ),

    /* 2.4 Package: Primitive_Traits (Introspection, Lifecycle & Built-ins) */
    PACKAGE(Traits,
        ENUM(Config, PRIMITIVE_TRAIT_CONFIG_TABLE),

        /* Introspection Predicates */
        PREDICATE(IsValid, (const MODULE_TYPE(Primitive, Traits) * traits)),
        PREDICATE(IsEqual, (const MODULE_TYPE(Primitive, Traits) * left,
                            const MODULE_TYPE(Primitive, Traits) * right)),
        PREDICATE(HasFlags, (const MODULE_TYPE(Primitive, Traits) * traits,
                             uint32_t mask)),
        PREDICATE(IsTrivial, (const MODULE_TYPE(Primitive, Traits) * traits)),
        PREDICATE(IsTrivialCopy, (const MODULE_TYPE(Primitive, Traits) * traits)),
        PREDICATE(IsTrivialCompare, (const MODULE_TYPE(Primitive, Traits) * traits)),
        PREDICATE(NeedsDispose, (const MODULE_TYPE(Primitive, Traits) * traits)),
        PREDICATE(IsNumeric, (const MODULE_TYPE(Primitive, Traits) * traits)),
        PREDICATE(IsSigned, (const MODULE_TYPE(Primitive, Traits) * traits)),
        PREDICATE(IsInteger, (const MODULE_TYPE(Primitive, Traits) * traits)),
        PREDICATE(IsFloat, (const MODULE_TYPE(Primitive, Traits) * traits)),
        PREDICATE(IsDynamic, (const MODULE_TYPE(Primitive, Traits) * traits)),

        /* Universal Type-Erased Lifecycle & Value Operations */
        OPERATION(Init, (const MODULE_TYPE(Primitive, Traits) * traits,
                         void *instance)),
        OPERATION(InitArray, (const MODULE_TYPE(Primitive, Traits) * traits,
                              void *array, size_t count)),
        PROCEDURE(Dispose, (const MODULE_TYPE(Primitive, Traits) * traits,
                            void *instance)),
        PROCEDURE(DisposeArray, (const MODULE_TYPE(Primitive, Traits) * traits,
                                 void *array, size_t count)),
        OPERATION(Copy, (const MODULE_TYPE(Primitive, Traits) * traits,
                         void *destination, const void *source)),
        OPERATION(CopyArray, (const MODULE_TYPE(Primitive, Traits) * traits,
                              void *destination, const void *source,
                              size_t count)),
        OPERATION(Move, (const MODULE_TYPE(Primitive, Traits) * traits,
                         void *destination, void *source)),
        OPERATION(MoveArray, (const MODULE_TYPE(Primitive, Traits) * traits,
                              void *destination, void *source, size_t count)),
        OPERATION(Swap, (const MODULE_TYPE(Primitive, Traits) * traits,
                         void *left, void *right)),
        COMPARISON(Compare, (const MODULE_TYPE(Primitive, Traits) * traits,
                             const void *left, const void *right)),
        PREDICATE(Equals, (const MODULE_TYPE(Primitive, Traits) * traits,
                           const void *left, const void *right)),
        FUNCTION(size_t, Hash, (const MODULE_TYPE(Primitive, Traits) * traits,
                                const void *instance)),
        FUNCTION(size_t, HashBytes, (const void *data, size_t size)),

        /* Built-in Primitive Trait Descriptor Accessors */
        PRIMITIVE_TYPE_TABLE(PRIMITIVE_TRAITS_PRIMITIVE_GETTER_DECL)
        FUNCTION(const MODULE_TYPE(Primitive, Traits) *, CString, (void))
    )
);

#undef PRIMITIVE_TRAITS_PRIMITIVE_GETTER_DECL

/* ========================================================================== */
/* 3. Implementation Include                                                  */
/* ========================================================================== */

#include "Impl/Traits.impl"

/* EOF */
