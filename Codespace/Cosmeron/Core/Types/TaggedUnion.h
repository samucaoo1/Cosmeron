#pragma once

#include "../Algorithm/Comparison.h"
#include "../Module/Module.h"
#include "../Condition/Status.h"
#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Operations.h"
#include "Traits.h"
#include "Primitive.h"
#include "TypeTable/Tag.impl"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* 1. Declarative Module: Type (Tag, ByteSlice, TaggedUnion, Package)    */
/* ========================================================================== */

#define COSMERON_MACRO_INTERNAL_TAGGED_UNION_FIELD(ROW)                        \
  PRIMITIVE_TYPE_GET(C_TYPE, ROW) PRIMITIVE_TYPE_GET(NAME, ROW);

#define COSMERON_MACRO_INTERNAL_TAGGED_UNION_PRIMITIVE_API(ROW)                \
  OPERATION(PP_OP_CAT2(Set_, PRIMITIVE_TYPE_GET(NAME, ROW)),                   \
            (MODULE_TYPE(Type, TaggedUnion) * self,                       \
             PRIMITIVE_TYPE_GET(C_TYPE, ROW) value)),                            \
  FUNCTION(MODULE_TYPE(Type, TaggedUnion),                                \
           PP_OP_CAT2(From_, PRIMITIVE_TYPE_GET(NAME, ROW)),                   \
           (PRIMITIVE_TYPE_GET(C_TYPE, ROW) value)),                             \
  OPERATION(PP_OP_CAT2(Get_, PRIMITIVE_TYPE_GET(NAME, ROW)),                   \
            (const MODULE_TYPE(Type, TaggedUnion) * self,                 \
             PRIMITIVE_TYPE_GET(C_TYPE, ROW) * outValue)),

MODULE(DOMAIN(Core), Type,
    /* 1.1 Closed Tag Enum */
    ENUM_EX(Tag, TYPE_TAG_TABLE, QUALIFIED_NAME),

    /* 1.2 Contiguous Byte Slice POD */
    STRUCT(ByteSlice,
        FIELDS(
            const void *data;
            size_t size;
        )
    ),

    /* 1.3 Internal Tagged Payload Union */
    ISTRUCT(TaggedPayload,
        FIELDS(
            union {
                PRIMITIVE_TYPE_TABLE(COSMERON_MACRO_INTERNAL_TAGGED_UNION_FIELD)
                const char *cString;
                void *rawPointer;
                MODULE_TYPE(Type, ByteSlice) bytes;
            } as;
        )
    ),

    /* 1.4 Canonical Pure POD TaggedUnion (TTaggedUnion) */
    STRUCT(TaggedUnion,
        FIELDS(
            MODULE_ENUM(Type, Tag) tag;
            MODULE_ITYPE(Type, TaggedPayload) value;
        )
    ),

    /* 1.5 Package: Type_TaggedUnion */
    PACKAGE(TaggedUnion,
        /* Lifecycle */
        PROCEDURE(Init, (MODULE_TYPE(Type, TaggedUnion) * self)),
        PROCEDURE(Clear, (MODULE_TYPE(Type, TaggedUnion) * self)),

        /* Typed Setters, Value Constructors & Strict Getters (All Primitives) */
        PRIMITIVE_TYPE_TABLE(COSMERON_MACRO_INTERNAL_TAGGED_UNION_PRIMITIVE_API)

        /* Special Value Setters, Constructors & Getters */
        OPERATION(Set_CString, (MODULE_TYPE(Type, TaggedUnion) * self,
                                const char *value)),
        FUNCTION(MODULE_TYPE(Type, TaggedUnion), From_CString,
                 (const char *value)),
        OPERATION(Get_CString, (const MODULE_TYPE(Type, TaggedUnion) * self,
                                const char **outValue)),

        OPERATION(Set_Bytes, (MODULE_TYPE(Type, TaggedUnion) * self,
                              const void *data, size_t size)),
        FUNCTION(MODULE_TYPE(Type, TaggedUnion), From_Bytes,
                 (const void *data, size_t size)),
        OPERATION(Get_Bytes, (const MODULE_TYPE(Type, TaggedUnion) * self,
                              MODULE_TYPE(Type, ByteSlice) * outValue)),

        OPERATION(Set_RawPointer, (MODULE_TYPE(Type, TaggedUnion) * self,
                                   void *pointer)),
        FUNCTION(MODULE_TYPE(Type, TaggedUnion), From_RawPointer,
                 (void *pointer)),
        OPERATION(Get_RawPointer,
                  (const MODULE_TYPE(Type, TaggedUnion) * self,
                   void **outValue)),

        /* Safe Numeric Coercions (Overflow-Checked) */
        VARIANT(Coerce,
            OPERATION(Int64, (const MODULE_TYPE(Type, TaggedUnion) * self,
                              int64_t *outValue)),
            OPERATION(UInt64, (const MODULE_TYPE(Type, TaggedUnion) * self,
                               uint64_t *outValue)),
            OPERATION(Float64, (const MODULE_TYPE(Type, TaggedUnion) * self,
                                double *outValue))
        ),

        /* Introspection */
        VARIANT(Is,
            PREDICATE(Empty, (const MODULE_TYPE(Type, TaggedUnion) * self)),
            PREDICATE(Tag, (const MODULE_TYPE(Type, TaggedUnion) * self,
                            MODULE_ENUM(Type, Tag) tag)),
            PREDICATE(Numeric, (const MODULE_TYPE(Type, TaggedUnion) * self)),
            PREDICATE(Integer, (const MODULE_TYPE(Type, TaggedUnion) * self)),
            PREDICATE(Signed, (const MODULE_TYPE(Type, TaggedUnion) * self)),
            PREDICATE(Float, (const MODULE_TYPE(Type, TaggedUnion) * self))
        ),
        FUNCTION(const char *, TagName, (MODULE_ENUM(Type, Tag) tag)),
        FUNCTION(MODULE_ENUM(Type, Category), CategoryOf,
                 (const MODULE_TYPE(Type, TaggedUnion) * self)),
        FUNCTION(const MODULE_TYPE(Type, Traits) *, TraitsOf,
                 (const MODULE_TYPE(Type, TaggedUnion) * self)),
        FUNCTION(const void *, PayloadPointer,
                 (const MODULE_TYPE(Type, TaggedUnion) * self)),

        /* Comparison, Equality, Hashing & Self-Traits */
        COMPARISON(Compare, (const MODULE_TYPE(Type, TaggedUnion) * left,
                             const MODULE_TYPE(Type, TaggedUnion) * right)),
        PREDICATE(Equals, (const MODULE_TYPE(Type, TaggedUnion) * left,
                           const MODULE_TYPE(Type, TaggedUnion) * right)),
        FUNCTION(size_t, Hash, (const MODULE_TYPE(Type, TaggedUnion) * self)),
        FUNCTION(const MODULE_TYPE(Type, Traits) *, Traits, (void))
    )
);

#undef COSMERON_MACRO_INTERNAL_TAGGED_UNION_FIELD
#undef COSMERON_MACRO_INTERNAL_TAGGED_UNION_PRIMITIVE_API

/* ========================================================================== */
/* 2. Implementation Include                                                  */
/* ========================================================================== */

#include "Impl/TaggedUnion.impl"

/* EOF */

