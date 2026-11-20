#pragma once

#include "../Condition/Status.h"
#include "../Module/Module.h"
#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Operations.h"
#include "Impl/Common.inc"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* ========================================================================== */
/* 1. X-Macro Tables and Enums                                                */
/* ========================================================================== */

#define STREAM_SEEK_ORIGIN_COL_NAME(NAME, VALUE, DESC) NAME
#define STREAM_SEEK_ORIGIN_COL_VALUE(NAME, VALUE, DESC) VALUE
#define STREAM_SEEK_ORIGIN_COL_DESC(NAME, VALUE, DESC) DESC
#define STREAM_SEEK_ORIGIN_GET(FIELD, ROW) PP_OP_CAT2(STREAM_SEEK_ORIGIN_COL_, FIELD) ROW

#define STREAM_SEEK_ORIGIN_TABLE(X) \
  X((SEEK_BEGIN,   0, "Beginning of stream")) \
  X((SEEK_CURRENT, 1, "Current stream position")) \
  X((SEEK_END,     2, "End of stream"))

/* ========================================================================== */
/* 2. Declarative TMemoryStream Object                                        */
/* ========================================================================== */

#if defined(MEMORY_DISABLE_FUNCTION_TABLE) &&                                  \
    !defined(COSMERON_DISABLE_FT_Memory)
#define COSMERON_DISABLE_FT_Memory 1
#endif
#if defined(MEMORY_STREAM_DISABLE_FUNCTION_TABLE) &&                           \
    !defined(COSMERON_DISABLE_FT_Memory_Stream)
#define COSMERON_DISABLE_FT_Memory_Stream 1
#endif

#define MEMORY_STREAM_INTERNAL_COL_NAME(NAME, VALUE) NAME
#define MEMORY_STREAM_INTERNAL_COL_VALUE(NAME, VALUE) VALUE
#define MEMORY_STREAM_INTERNAL_GET(FIELD, ROW)                                 \
  PP_OP_CAT2(MEMORY_STREAM_INTERNAL_COL_, FIELD) ROW

#define MEMORY_STREAM_INTERNAL_TABLE(X)                                        \
  X((BITS_PER_BYTE, 8U))                                                       \
  X((MSB_BIT_INDEX, 7U))                                                       \
  X((MAX_BIT_COUNT, 64U))                                                      \
  X((BYTE_MASK, 0xFFU))

IENUM_DEF(Memory_Stream, Config, MEMORY_STREAM_INTERNAL_TABLE)

MODULE(DOMAIN(Core), Memory,
    OBJECT(Stream,
        FIELDS(
            uint8_t *data;
            size_t capacity;
            size_t offset;
            uint8_t bitOffset;
            bool isReadOnly;
        ),
        ENUM(SeekOrigin, STREAM_SEEK_ORIGIN_TABLE),
        VARIANT(Init,
            OPERATION(Read, (const void *buffer, size_t size)),
            OPERATION(Write, (void *buffer, size_t capacity))
        ),
        CONST_FUNCTION_INLINE(size_t, Remaining, (), {
          if (!self || self->offset >= self->capacity) return 0U;
          return self->capacity - self->offset;
        }),
        CONST_FUNCTION_INLINE(size_t, RemainingBits, (), {
          if (!self || self->offset >= self->capacity) return 0U;
          size_t bytes = self->capacity - self->offset;
          return (bytes * (size_t)CNS(Memory, Stream, Internal, BITS_PER_BYTE)) -
                 (size_t)self->bitOffset;
        }),
        CONST_FUNCTION_INLINE(size_t, Position, (), {
          return self ? self->offset : 0U;
        }),
        CONST_FUNCTION_INLINE(uint8_t, BitPosition, (), {
          return self ? self->bitOffset : 0U;
        }),
        CONST_FUNCTION_INLINE(size_t, Capacity, (), {
          return self ? self->capacity : 0U;
        }),
        CONST_PREDICATE_INLINE(IsAtEnd, (), {
          return !self || (self->offset >= self->capacity && self->bitOffset == 0U);
        }),
        CONST_PREDICATE_INLINE(IsReadOnly, (), {
          return !self || self->isReadOnly;
        }),
        CONST_PREDICATE_INLINE(HasRemaining, (size_t count), {
          return self && (self->capacity >= self->offset) && ((self->capacity - self->offset) >= count);
        }),
        CONST_PREDICATE_INLINE(HasRemainingBits, (size_t bitCount), {
          if (!self || self->offset >= self->capacity) return false;
          size_t bits =
              ((self->capacity - self->offset) *
               (size_t)CNS(Memory, Stream, Internal, BITS_PER_BYTE)) -
              (size_t)self->bitOffset;
          return bits >= bitCount;
        }),
        CONST_FUNCTION_INLINE(const void *, Data, (), {
          return self ? (const void *)self->data : NULL;
        }),
        CONST_FUNCTION_INLINE(const void *, Current, (), {
          if (!self || !self->data || self->offset >= self->capacity) return NULL;
          return (const void *)(self->data + self->offset);
        }),
        OPERATION(Seek, (int64_t offset, MODULE_ENUM(Memory, Stream, SeekOrigin) origin)),
        OPERATION(Skip, (size_t count)),
        OPERATION(Rewind, ()),
        OPERATION(AlignToByte, ()),
        CONST_OPERATION(Slice, (size_t size, MODULE_TYPE(Memory, Stream) *outSubStream)),
        VARIANT(Read,
            OPERATION(Slice, (size_t size, MODULE_TYPE(Memory, Stream) *outSubStream)),
            OPERATION(Bytes, (void *destination, size_t size)),
            OPERATION(Bits, (uint8_t bitCount, uint64_t *outValue)),
            OPERATION(UInt8, (uint8_t *outValue)),
            OPERATION(Int8, (int8_t *outValue)),
            OPERATION(UInt16LE, (uint16_t *outValue)),
            OPERATION(UInt16BE, (uint16_t *outValue)),
            OPERATION(Int16LE, (int16_t *outValue)),
            OPERATION(Int16BE, (int16_t *outValue)),
            OPERATION(UInt32LE, (uint32_t *outValue)),
            OPERATION(UInt32BE, (uint32_t *outValue)),
            OPERATION(Int32LE, (int32_t *outValue)),
            OPERATION(Int32BE, (int32_t *outValue)),
            OPERATION(UInt64LE, (uint64_t *outValue)),
            OPERATION(UInt64BE, (uint64_t *outValue)),
            OPERATION(Int64LE, (int64_t *outValue)),
            OPERATION(Int64BE, (int64_t *outValue))
        ),
        VARIANT(Peek,
            CONST_OPERATION(Bytes, (void *destination, size_t size)),
            CONST_OPERATION(UInt8, (uint8_t *outValue))
        ),
        VARIANT(Write,
            OPERATION(Bytes, (const void *source, size_t size)),
            OPERATION(Bits, (uint64_t value, uint8_t bitCount)),
            OPERATION(UInt8, (uint8_t value)),
            OPERATION(Int8, (int8_t value)),
            OPERATION(UInt16LE, (uint16_t value)),
            OPERATION(UInt16BE, (uint16_t value)),
            OPERATION(Int16LE, (int16_t value)),
            OPERATION(Int16BE, (int16_t value)),
            OPERATION(UInt32LE, (uint32_t value)),
            OPERATION(UInt32BE, (uint32_t value)),
            OPERATION(Int32LE, (int32_t value)),
            OPERATION(Int32BE, (int32_t value)),
            OPERATION(UInt64LE, (uint64_t value)),
            OPERATION(UInt64BE, (uint64_t value)),
            OPERATION(Int64LE, (int64_t value)),
            OPERATION(Int64BE, (int64_t value))
        )
    )
);

#include "Impl/Stream.impl"

/* EOF */
