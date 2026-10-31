#pragma once

#include "../Module/Object.h"
#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Operations.h"
#include "Memory.space"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define STREAM_MOD GNS2(MEMORY_MOD, Stream)
#define STREAM_CMOD PP_OP_CAT3(MEMORY_CMOD, _, STREAM)

#define STREAM_NS(NAME) GNS3(LIB_PREFIX(MEMORY_MOD), Stream, NAME)
#define STREAM_CNS(NAME) CNS3(LIB_PREFIX_CONST(MEMORY_CMOD), STREAM, NAME)
#define STREAM_INS(NAME) GNS4(LIB_PREFIX(MEMORY_MOD), Stream, Internal, NAME)
#define STREAM_CINS(NAME) CNS4(LIB_PREFIX_CONST(MEMORY_CMOD), STREAM, INTERNAL, NAME)

#define STREAM_TYPE(NAME) STREAM_NS(NAME)
#define STREAM_FUNC(NAME) GNS2(STREAM_TYPE(TStream), NAME)
#define STREAM_CONST(NAME) STREAM_CNS(NAME)

#define STREAM_SEEK_ORIGIN_COL_NAME(NAME, VALUE, DESC) NAME
#define STREAM_SEEK_ORIGIN_COL_VALUE(NAME, VALUE, DESC) VALUE
#define STREAM_SEEK_ORIGIN_COL_DESC(NAME, VALUE, DESC) DESC
#define STREAM_SEEK_ORIGIN_GET(FIELD, ROW) PP_OP_CAT2(STREAM_SEEK_ORIGIN_COL_, FIELD) ROW

#define STREAM_SEEK_ORIGIN_TABLE(X) \
  X((SEEK_BEGIN,   0, "Beginning of stream")) \
  X((SEEK_CURRENT, 1, "Current stream position")) \
  X((SEEK_END,     2, "End of stream"))

typedef enum STREAM_TYPE(SeekOrigin) {
#define STREAM_SEEK_ORIGIN_ENUM_ITEM(ROW) \
  STREAM_CONST(STREAM_SEEK_ORIGIN_GET(NAME, ROW)) = STREAM_SEEK_ORIGIN_GET(VALUE, ROW),
  STREAM_SEEK_ORIGIN_TABLE(STREAM_SEEK_ORIGIN_ENUM_ITEM)
#undef STREAM_SEEK_ORIGIN_ENUM_ITEM
} STREAM_TYPE(SeekOrigin);

typedef STREAM_TYPE(SeekOrigin) MEMORY_TYPE(StreamSeekOrigin);

OBJECT(STREAM_TYPE(TStream),
    FIELDS(
        uint8_t *data;
        size_t capacity;
        size_t offset;
        uint8_t bitOffset;
        bool isReadOnly;
    ),
    OPERATION(InitRead, (const void *buffer, size_t size)),
    OPERATION(InitWrite, (void *buffer, size_t capacity)),
    CONST_FUNCTION_INLINE(size_t, Remaining, (), {
      if (!self || self->offset >= self->capacity) return 0U;
      return self->capacity - self->offset;
    }),
    CONST_FUNCTION_INLINE(size_t, RemainingBits, (), {
      if (!self || self->offset >= self->capacity) return 0U;
      size_t bytes = self->capacity - self->offset;
      return (bytes * 8U) - (size_t)self->bitOffset;
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
      size_t bits = ((self->capacity - self->offset) * 8U) - (size_t)self->bitOffset;
      return bits >= bitCount;
    }),
    CONST_FUNCTION_INLINE(const void *, Data, (), {
      return self ? (const void *)self->data : NULL;
    }),
    CONST_FUNCTION_INLINE(const void *, Current, (), {
      if (!self || !self->data || self->offset >= self->capacity) return NULL;
      return (const void *)(self->data + self->offset);
    }),
    OPERATION(Seek, (int64_t offset, STREAM_TYPE(SeekOrigin) origin)),
    OPERATION(Skip, (size_t count)),
    OPERATION(Rewind, ()),
    OPERATION(AlignToByte, ()),
    CONST_OPERATION(Slice, (size_t size, STREAM_TYPE(TStream) *outSubStream)),
    OPERATION(ReadSlice, (size_t size, STREAM_TYPE(TStream) *outSubStream)),
    OPERATION(ReadBytes, (void *destination, size_t size)),
    CONST_OPERATION(PeekBytes, (void *destination, size_t size)),
    OPERATION(ReadBits, (uint8_t bitCount, uint64_t *outValue)),
    OPERATION(WriteBits, (uint64_t value, uint8_t bitCount)),
    OPERATION(WriteBytes, (const void *source, size_t size)),
    OPERATION(ReadUInt8, (uint8_t *outValue)),
    OPERATION(ReadInt8, (int8_t *outValue)),
    CONST_OPERATION(PeekUInt8, (uint8_t *outValue)),
    OPERATION(WriteUInt8, (uint8_t value)),
    OPERATION(WriteInt8, (int8_t value)),
    OPERATION(ReadUInt16LE, (uint16_t *outValue)),
    OPERATION(ReadUInt16BE, (uint16_t *outValue)),
    OPERATION(ReadInt16LE, (int16_t *outValue)),
    OPERATION(ReadInt16BE, (int16_t *outValue)),
    OPERATION(WriteUInt16LE, (uint16_t value)),
    OPERATION(WriteUInt16BE, (uint16_t value)),
    OPERATION(WriteInt16LE, (int16_t value)),
    OPERATION(WriteInt16BE, (int16_t value)),
    OPERATION(ReadUInt32LE, (uint32_t *outValue)),
    OPERATION(ReadUInt32BE, (uint32_t *outValue)),
    OPERATION(ReadInt32LE, (int32_t *outValue)),
    OPERATION(ReadInt32BE, (int32_t *outValue)),
    OPERATION(WriteUInt32LE, (uint32_t value)),
    OPERATION(WriteUInt32BE, (uint32_t value)),
    OPERATION(WriteInt32LE, (int32_t value)),
    OPERATION(WriteInt32BE, (int32_t value)),
    OPERATION(ReadUInt64LE, (uint64_t *outValue)),
    OPERATION(ReadUInt64BE, (uint64_t *outValue)),
    OPERATION(ReadInt64LE, (int64_t *outValue)),
    OPERATION(ReadInt64BE, (int64_t *outValue)),
    OPERATION(WriteUInt64LE, (uint64_t value)),
    OPERATION(WriteUInt64BE, (uint64_t value)),
    OPERATION(WriteInt64LE, (int64_t value)),
    OPERATION(WriteInt64BE, (int64_t value))
);

typedef STREAM_TYPE(TStream) MEMORY_TYPE(TStream);
typedef STREAM_TYPE(TStream) STREAM_TYPE(TMemoryStream);
typedef STREAM_TYPE(TStream) MEMORY_TYPE(TMemoryStream);

#include "Impl/Stream.impl"
/* EOF */
