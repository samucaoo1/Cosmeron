#pragma once

#include "../Algorithm/Comparison.h"
#include "../Condition/Status.h"
#include "../Module/Module.h"
#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Detect/Detect.h"
#include "../Preprocessor/Operations.h"
#include "Primitive.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* 1. Trait Configuration Table                                               */
/* ========================================================================== */

#define TYPE_TRAIT_CONFIG_COL_NAME(NAME, VALUE) NAME
#define TYPE_TRAIT_CONFIG_COL_VALUE(NAME, VALUE) VALUE
#define TYPE_TRAIT_CONFIG_GET(FIELD, ROW)                                      \
  PP_OP_CAT2(TYPE_TRAIT_CONFIG_COL_, FIELD) ROW

#define TYPE_TRAIT_CONFIG_TABLE(X) X((SWAP_BUFFER_SIZE, 64U))

/* ========================================================================== */
/* 2. Declarative Module: Type (TraitFlag, Callbacks, TTraits, Package)       */
/* ========================================================================== */

#define COSMERON_MACRO_INTERNAL_TYPE_TRAITS_PRIMITIVE_GETTER_DECL(ROW)         \
  FUNCTION(const MODULE_TYPE(Type, Traits) *, PRIMITIVE_TYPE_GET(NAME, ROW),   \
           (void)),

MODULE(
    DOMAIN(Core), Type,

    /* 2.1 Trait Behavioral Flags Enum */
    ENUM_EX(TraitFlag, TYPE_TRAIT_FLAG_TABLE, QUALIFIED),

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
            MODULE_ENUM(Type, Category) category;
            uint32_t flags;
            MODULE_CALLBACK(Type, TraitInit) init;
            MODULE_CALLBACK(Type, TraitDispose) dispose;
            MODULE_CALLBACK(Type, TraitCopy) copy;
            MODULE_CALLBACK(Type, TraitMove) move;
            MODULE_CALLBACK(Type, TraitCompare) compare;
            MODULE_CALLBACK(Type, TraitHash) hash;
        )
    ),

    /* 2.4 Package: Type_Traits (Introspection, Lifecycle & Built-ins) */
    PACKAGE(Traits,
        ENUM(Config, TYPE_TRAIT_CONFIG_TABLE),

        /* Introspection Predicates */
        VARIANT(Is,
            PREDICATE(Valid, (const MODULE_TYPE(Type, Traits) * traits)),
            PREDICATE(Equal, (const MODULE_TYPE(Type, Traits) * left,
                              const MODULE_TYPE(Type, Traits) * right)),
            PREDICATE(Trivial, (const MODULE_TYPE(Type, Traits) * traits)),
            PREDICATE(TrivialCopy, (const MODULE_TYPE(Type, Traits) * traits)),
            PREDICATE(TrivialCompare, (const MODULE_TYPE(Type, Traits) * traits)),
            PREDICATE(Numeric, (const MODULE_TYPE(Type, Traits) * traits)),
            PREDICATE(Signed, (const MODULE_TYPE(Type, Traits) * traits)),
            PREDICATE(Integer, (const MODULE_TYPE(Type, Traits) * traits)),
            PREDICATE(Float, (const MODULE_TYPE(Type, Traits) * traits)),
            PREDICATE(Dynamic, (const MODULE_TYPE(Type, Traits) * traits))
        ),
        PREDICATE(HasFlags,
                  (const MODULE_TYPE(Type, Traits) * traits, uint32_t mask)),
        PREDICATE(NeedsDispose, (const MODULE_TYPE(Type, Traits) * traits)),

        /* Universal Type-Erased Lifecycle & Value Operations */
        VARIANT(Init,
            OPERATION((const MODULE_TYPE(Type, Traits) * traits, void *instance)),
            OPERATION(Array, (const MODULE_TYPE(Type, Traits) * traits,
                              void *array, size_t count))
        ),
        VARIANT(Dispose,
            PROCEDURE((const MODULE_TYPE(Type, Traits) * traits, void *instance)),
            PROCEDURE(Array, (const MODULE_TYPE(Type, Traits) * traits,
                              void *array, size_t count))
        ),
        VARIANT(Copy,
            OPERATION((const MODULE_TYPE(Type, Traits) * traits,
                       void *destination, const void *source)),
            OPERATION(Array, (const MODULE_TYPE(Type, Traits) * traits,
                              void *destination, const void *source,
                              size_t count))
        ),
        VARIANT(Move,
            OPERATION((const MODULE_TYPE(Type, Traits) * traits,
                       void *destination, void *source)),
            OPERATION(Array, (const MODULE_TYPE(Type, Traits) * traits,
                              void *destination, void *source, size_t count))
        ),
        OPERATION(Swap, (const MODULE_TYPE(Type, Traits) * traits, void *left,
                         void *right)),
        COMPARISON(Compare, (const MODULE_TYPE(Type, Traits) * traits,
                             const void *left, const void *right)),
        PREDICATE(Equals, (const MODULE_TYPE(Type, Traits) * traits,
                           const void *left, const void *right)),
        VARIANT(Hash,
            FUNCTION(size_t, , (const MODULE_TYPE(Type, Traits) * traits,
                                const void *instance)),
            FUNCTION(size_t, Bytes, (const void *data, size_t size))
        ),

        /* Built-in Primitive Trait Descriptor Accessors */
        PRIMITIVE_TYPE_TABLE(
            COSMERON_MACRO_INTERNAL_TYPE_TRAITS_PRIMITIVE_GETTER_DECL)
        FUNCTION(const MODULE_TYPE(Type, Traits) *, CString, (void))
    )
);

#undef COSMERON_MACRO_INTERNAL_TYPE_TRAITS_PRIMITIVE_GETTER_DECL

/* ========================================================================== */
/* 3. Implementation Include                                                  */
/* ========================================================================== */

#include "Impl/Traits.impl"

/* EOF */
