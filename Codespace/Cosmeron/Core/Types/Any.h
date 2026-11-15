#pragma once

#include "../Algorithm/Comparison.h"
#include "../Memory/Alloc.h"
#include "../Module/Module.h"
#include "../Condition/Status.h"
#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Operations.h"
#include "TaggedUnion.h"
#include "Traits.h"
#include "Primitive.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* 1. Any Configuration & Size Specialization Tables                          */
/* ========================================================================== */

#define TYPE_ANY_CONFIG_COL_NAME(NAME, VALUE)  NAME
#define TYPE_ANY_CONFIG_COL_VALUE(NAME, VALUE) VALUE
#define TYPE_ANY_CONFIG_GET(FIELD, ROW)                                   \
  PP_OP_CAT2(TYPE_ANY_CONFIG_COL_, FIELD) ROW

#define TYPE_ANY_CONFIG_TABLE(X)                                          \
  X((SBO_CAPACITY, 16))

#define TYPE_ANY_SIZE_COL_NAME(NAME, SBO_BYTES)      NAME
#define TYPE_ANY_SIZE_COL_SBO_BYTES(NAME, SBO_BYTES) SBO_BYTES
#define TYPE_ANY_SIZE_GET(FIELD, ROW)                                     \
  PP_OP_CAT2(TYPE_ANY_SIZE_COL_, FIELD) ROW

#define TYPE_ANY_SIZE_TABLE(X)                                            \
  X((Any32,  32))                                                              \
  X((Any64,  64))                                                              \
  X((Any128, 128))                                                             \
  X((Any256, 256))

/* ========================================================================== */
/* 2. Declarative Module: Type (AnyStorage, TAny, AnyConversion)         */
/* ========================================================================== */

#define COSMERON_MACRO_INTERNAL_ANY_PRIMITIVE_API(ROW)                         \
  OPERATION(PP_OP_CAT2(Set_, PRIMITIVE_TYPE_GET(NAME, ROW)),                   \
            (MODULE_TYPE(Type, Any) * self,                               \
             PRIMITIVE_TYPE_GET(TYPE, ROW) value)),                            \
  OPERATION(PP_OP_CAT2(Get_, PRIMITIVE_TYPE_GET(NAME, ROW)),                   \
            (const MODULE_TYPE(Type, Any) * self,                         \
             PRIMITIVE_TYPE_GET(TYPE, ROW) * outValue)),

MODULE(DOMAIN(Core), Type,
    /* 2.1 Internal SBO / Heap Storage Union (Default 16B) */
    ISTRUCT(AnyStorage,
        FIELDS(
            union {
                void *heapPointer;
                max_align_t alignDummy;
                unsigned char inlineBytes[16];
            } as;
        )
    ),

    /* 2.2 Canonical Pure POD Type-Erased Box (TAny, 16B SBO) */
    STRUCT(Any,
        FIELDS(
            const MODULE_TYPE(Type, Traits) *traits;
            MODULE_ITYPE(Type, AnyStorage) storage;
        )
    ),

    /* 2.3 Package: Type_AnyConversion (Shared Engine & Cross-Size Conversion) */
    PACKAGE(AnyConversion,
        PREDICATE(FitsInline, (const MODULE_TYPE(Type, Traits) * traits,
                               size_t sboCapacity)),
        FUNCTION(const void *, RawPointerOf,
                 (const MODULE_TYPE(Type, Traits) * traits,
                  void *heapPointer, const unsigned char *inlineBytes,
                  size_t sboCapacity)),
        FUNCTION(void *, MutableRawPointerOf,
                 (const MODULE_TYPE(Type, Traits) * traits,
                  void *heapPointer, unsigned char *inlineBytes,
                  size_t sboCapacity)),
        PROCEDURE(DisposeRaw,
                  (const MODULE_TYPE(Type, Traits) **traitsSlot,
                   void **heapSlot, unsigned char *inlineBytes,
                   size_t sboCapacity)),
        OPERATION(EmplaceRaw,
                  (const MODULE_TYPE(Type, Traits) **traitsSlot,
                   void **heapSlot, unsigned char *inlineBytes,
                   size_t sboCapacity,
                   const MODULE_TYPE(Type, Traits) * traits,
                   const void *sourceInstance)),
        OPERATION(MoveFromRaw,
                  (const MODULE_TYPE(Type, Traits) **traitsSlot,
                   void **heapSlot, unsigned char *inlineBytes,
                   size_t sboCapacity,
                   const MODULE_TYPE(Type, Traits) * traits,
                   void *sourceInstance)),
        OPERATION(Clone,
                  (const MODULE_TYPE(Type, Traits) **destTraits,
                   void **destHeap, unsigned char *destInline, size_t destSbo,
                   const MODULE_TYPE(Type, Traits) *const *srcTraits,
                   void *const *srcHeap, const unsigned char *srcInline,
                   size_t srcSbo)),
        OPERATION(Move,
                  (const MODULE_TYPE(Type, Traits) **destTraits,
                   void **destHeap, unsigned char *destInline, size_t destSbo,
                   const MODULE_TYPE(Type, Traits) **srcTraits,
                   void **srcHeap, unsigned char *srcInline, size_t srcSbo)),
        OPERATION(SwapRaw,
                  (const MODULE_TYPE(Type, Traits) **leftTraits,
                   void **leftHeap, unsigned char *leftInline, size_t leftSbo,
                   const MODULE_TYPE(Type, Traits) **rightTraits,
                   void **rightHeap, unsigned char *rightInline,
                   size_t rightSbo)),
        OPERATION(FromTaggedUnionRaw,
                  (const MODULE_TYPE(Type, Traits) **traitsSlot,
                   void **heapSlot, unsigned char *inlineBytes,
                   size_t sboCapacity,
                   const MODULE_TYPE(Type, TaggedUnion) * source)),
        OPERATION(ToTaggedUnionRaw,
                  (const MODULE_TYPE(Type, Traits) * traits,
                   void *heapPointer, const unsigned char *inlineBytes,
                   size_t sboCapacity,
                   MODULE_TYPE(Type, TaggedUnion) * outUnion)),
        COMPARISON(CompareRaw,
                   (const MODULE_TYPE(Type, Traits) * leftTraits,
                    void *leftHeap, const unsigned char *leftInline,
                    size_t leftSbo,
                    const MODULE_TYPE(Type, Traits) * rightTraits,
                    void *rightHeap, const unsigned char *rightInline,
                    size_t rightSbo)),
        PREDICATE(EqualsRaw,
                  (const MODULE_TYPE(Type, Traits) * leftTraits,
                   void *leftHeap, const unsigned char *leftInline,
                   size_t leftSbo,
                   const MODULE_TYPE(Type, Traits) * rightTraits,
                   void *rightHeap, const unsigned char *rightInline,
                   size_t rightSbo)),
        FUNCTION(size_t, HashRaw,
                 (const MODULE_TYPE(Type, Traits) * traits,
                  void *heapPointer, const unsigned char *inlineBytes,
                  size_t sboCapacity))
    ),

    /* 2.4 Package: Type_Any */
    PACKAGE(Any,
        ENUM(Config, TYPE_ANY_CONFIG_TABLE),

        /* Lifecycle */
        PROCEDURE(Init, (MODULE_TYPE(Type, Any) * self)),
        PROCEDURE(Reset, (MODULE_TYPE(Type, Any) * self)),
        PROCEDURE(Dispose, (MODULE_TYPE(Type, Any) * self)),

        /* Construction, Cloning, Moving & Swapping */
        OPERATION(Emplace, (MODULE_TYPE(Type, Any) * self,
                            const MODULE_TYPE(Type, Traits) * traits,
                            const void *sourceInstance)),
        OPERATION(MoveFrom, (MODULE_TYPE(Type, Any) * self,
                             const MODULE_TYPE(Type, Traits) * traits,
                             void *sourceInstance)),
        OPERATION(Clone, (MODULE_TYPE(Type, Any) * destination,
                          const MODULE_TYPE(Type, Any) * source)),
        OPERATION(Move, (MODULE_TYPE(Type, Any) * destination,
                         MODULE_TYPE(Type, Any) * source)),
        OPERATION(Swap, (MODULE_TYPE(Type, Any) * left,
                         MODULE_TYPE(Type, Any) * right)),

        /* Introspection */
        PREDICATE(HasValue, (const MODULE_TYPE(Type, Any) * self)),
        PREDICATE(IsEmpty, (const MODULE_TYPE(Type, Any) * self)),
        PREDICATE(IsInline, (const MODULE_TYPE(Type, Any) * self)),
        PREDICATE(IsType, (const MODULE_TYPE(Type, Any) * self,
                           const MODULE_TYPE(Type, Traits) * expectedTraits)),
        FUNCTION(const MODULE_TYPE(Type, Traits) *, TraitsOf,
                 (const MODULE_TYPE(Type, Any) * self)),
        FUNCTION(size_t, Capacity, (void)),

        /* Type-Safe Extraction & Borrowing */
        FUNCTION(const void *, RawPointer,
                 (const MODULE_TYPE(Type, Any) * self)),
        FUNCTION(void *, MutableRawPointer,
                 (MODULE_TYPE(Type, Any) * self)),
        OPERATION(Get, (const MODULE_TYPE(Type, Any) * self,
                        const MODULE_TYPE(Type, Traits) * expectedTraits,
                        void *outValue)),
        OPERATION(Borrow, (const MODULE_TYPE(Type, Any) * self,
                           const MODULE_TYPE(Type, Traits) * expectedTraits,
                           const void **outPointer)),
        OPERATION(BorrowMutable,
                  (MODULE_TYPE(Type, Any) * self,
                   const MODULE_TYPE(Type, Traits) * expectedTraits,
                   void **outPointer)),

        /* Typed Primitive Shortcuts */
        PRIMITIVE_TYPE_TABLE(COSMERON_MACRO_INTERNAL_ANY_PRIMITIVE_API)

        /* TaggedUnion Interoperability */
        OPERATION(FromTaggedUnion,
                  (MODULE_TYPE(Type, Any) * self,
                   const MODULE_TYPE(Type, TaggedUnion) * source)),
        OPERATION(ToTaggedUnion,
                  (const MODULE_TYPE(Type, Any) * self,
                   MODULE_TYPE(Type, TaggedUnion) * outUnion)),

        /* Comparison, Equality, Hashing & Self-Traits */
        COMPARISON(Compare, (const MODULE_TYPE(Type, Any) * left,
                             const MODULE_TYPE(Type, Any) * right)),
        PREDICATE(Equals, (const MODULE_TYPE(Type, Any) * left,
                           const MODULE_TYPE(Type, Any) * right)),
        FUNCTION(size_t, Hash, (const MODULE_TYPE(Type, Any) * self)),
        FUNCTION(const MODULE_TYPE(Type, Traits) *, Traits, (void))
    )
);

#undef COSMERON_MACRO_INTERNAL_ANY_PRIMITIVE_API

/* ========================================================================== */
/* 3. Cross-Size Any Conversion Utilities                                     */
/* ========================================================================== */

#define Type_Any_Conversion_Clone(destPtr, srcPtr)                        \
  MODULE_FUNC(Type, AnyConversion, Clone)(                                \
      &(destPtr)->traits, &(destPtr)->storage.as.heapPointer,                  \
      (destPtr)->storage.as.inlineBytes,                                       \
      sizeof((destPtr)->storage.as.inlineBytes),                               \
      &(srcPtr)->traits, &(srcPtr)->storage.as.heapPointer,                    \
      (srcPtr)->storage.as.inlineBytes,                                        \
      sizeof((srcPtr)->storage.as.inlineBytes))

#define Type_Any_Conversion_Move(destPtr, srcPtr)                         \
  MODULE_FUNC(Type, AnyConversion, Move)(                                 \
      &(destPtr)->traits, &(destPtr)->storage.as.heapPointer,                  \
      (destPtr)->storage.as.inlineBytes,                                       \
      sizeof((destPtr)->storage.as.inlineBytes),                               \
      &(srcPtr)->traits, &(srcPtr)->storage.as.heapPointer,                    \
      (srcPtr)->storage.as.inlineBytes,                                        \
      sizeof((srcPtr)->storage.as.inlineBytes))

/* ========================================================================== */
/* 4. Implementation Include & SBO Size Specializations                       */
/* ========================================================================== */

#include "Impl/Any.impl"
#include "Any.def"

TEMPLATE_INSTANTIATE_TABLE(Type, Any, TYPE_ANY_SIZE_TABLE);

/* EOF */
