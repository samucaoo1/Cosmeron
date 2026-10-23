#pragma once

#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Operations.h"
#include "Memory.space"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define STREAM_NS(NAME) GNS2(MEMORY_NS(Stream), NAME)
#define STREAM_CNS(NAME) CNS3(LIB_PREFIX_CONST(MEMORY_CMOD), STREAM, NAME)

#define STREAM_TYPE(NAME) STREAM_NS(NAME)
#define STREAM_FUNC(NAME) STREAM_NS(NAME)
#define STREAM_CONST(NAME) STREAM_CNS(NAME)

typedef enum STREAM_TYPE(SeekOrigin) {
  STREAM_CONST(SEEK_BEGIN),
  STREAM_CONST(SEEK_CURRENT),
  STREAM_CONST(SEEK_END)
} STREAM_TYPE(SeekOrigin);

typedef STREAM_TYPE(SeekOrigin) MEMORY_TYPE(StreamSeekOrigin);

typedef struct STREAM_TYPE(TStream) {
  uint8_t *data;
  size_t capacity;
  size_t offset;
  uint8_t bitOffset;
  bool isReadOnly;
} STREAM_TYPE(TStream);

typedef STREAM_TYPE(TStream) MEMORY_TYPE(TStream);

#define STREAM_INIT_READ_PROTOTYPE                                             \
  static inline OPSTATUS STREAM_FUNC(InitRead)(                                \
      STREAM_TYPE(TStream) *stream, const void *buffer, size_t size)

#define STREAM_INIT_WRITE_PROTOTYPE                                            \
  static inline OPSTATUS STREAM_FUNC(InitWrite)(                               \
      STREAM_TYPE(TStream) *stream, void *buffer, size_t capacity)

#define STREAM_REMAINING_PROTOTYPE                                             \
  static inline size_t STREAM_FUNC(Remaining)(                                 \
      const STREAM_TYPE(TStream) *stream)

#define STREAM_REMAINING_BITS_PROTOTYPE                                        \
  static inline size_t STREAM_FUNC(RemainingBits)(                             \
      const STREAM_TYPE(TStream) *stream)

#define STREAM_POSITION_PROTOTYPE                                              \
  static inline size_t STREAM_FUNC(Position)(                                  \
      const STREAM_TYPE(TStream) *stream)

#define STREAM_BIT_POSITION_PROTOTYPE                                          \
  static inline uint8_t STREAM_FUNC(BitPosition)(                              \
      const STREAM_TYPE(TStream) *stream)

#define STREAM_CAPACITY_PROTOTYPE                                              \
  static inline size_t STREAM_FUNC(Capacity)(                                  \
      const STREAM_TYPE(TStream) *stream)

#define STREAM_IS_AT_END_PROTOTYPE                                             \
  static inline bool STREAM_FUNC(IsAtEnd)(                                     \
      const STREAM_TYPE(TStream) *stream)

#define STREAM_IS_READ_ONLY_PROTOTYPE                                          \
  static inline bool STREAM_FUNC(IsReadOnly)(                                  \
      const STREAM_TYPE(TStream) *stream)

#define STREAM_HAS_REMAINING_PROTOTYPE                                         \
  static inline bool STREAM_FUNC(HasRemaining)(                                \
      const STREAM_TYPE(TStream) *stream, size_t count)

#define STREAM_HAS_REMAINING_BITS_PROTOTYPE                                    \
  static inline bool STREAM_FUNC(HasRemainingBits)(                            \
      const STREAM_TYPE(TStream) *stream, size_t bitCount)

#define STREAM_DATA_PROTOTYPE                                                  \
  static inline const void *STREAM_FUNC(Data)(                                 \
      const STREAM_TYPE(TStream) *stream)

#define STREAM_CURRENT_PROTOTYPE                                               \
  static inline const void *STREAM_FUNC(Current)(                              \
      const STREAM_TYPE(TStream) *stream)

#define STREAM_SEEK_PROTOTYPE                                                  \
  static inline OPSTATUS STREAM_FUNC(Seek)(                                    \
      STREAM_TYPE(TStream) *stream, int64_t offset,                            \
      STREAM_TYPE(SeekOrigin) origin)

#define STREAM_SKIP_PROTOTYPE                                                  \
  static inline OPSTATUS STREAM_FUNC(Skip)(                                    \
      STREAM_TYPE(TStream) *stream, size_t count)

#define STREAM_REWIND_PROTOTYPE                                                \
  static inline OPSTATUS STREAM_FUNC(Rewind)(                                  \
      STREAM_TYPE(TStream) *stream)

#define STREAM_ALIGN_TO_BYTE_PROTOTYPE                                         \
  static inline OPSTATUS STREAM_FUNC(AlignToByte)(                             \
      STREAM_TYPE(TStream) *stream)

#define STREAM_SLICE_PROTOTYPE                                                 \
  static inline OPSTATUS STREAM_FUNC(Slice)(                                   \
      const STREAM_TYPE(TStream) *stream, size_t size,                         \
      STREAM_TYPE(TStream) *outSubStream)

#define STREAM_READ_SLICE_PROTOTYPE                                            \
  static inline OPSTATUS STREAM_FUNC(ReadSlice)(                               \
      STREAM_TYPE(TStream) *stream, size_t size,                               \
      STREAM_TYPE(TStream) *outSubStream)

#define STREAM_READ_BYTES_PROTOTYPE                                            \
  static inline OPSTATUS STREAM_FUNC(ReadBytes)(                               \
      STREAM_TYPE(TStream) *stream, void *destination, size_t size)

#define STREAM_PEEK_BYTES_PROTOTYPE                                            \
  static inline OPSTATUS STREAM_FUNC(PeekBytes)(                               \
      const STREAM_TYPE(TStream) *stream, void *destination, size_t size)

#define STREAM_READ_BITS_PROTOTYPE                                             \
  static inline OPSTATUS STREAM_FUNC(ReadBits)(                                \
      STREAM_TYPE(TStream) *stream, uint8_t bitCount, uint64_t *outValue)

#define STREAM_WRITE_BITS_PROTOTYPE                                            \
  static inline OPSTATUS STREAM_FUNC(WriteBits)(                               \
      STREAM_TYPE(TStream) *stream, uint64_t value, uint8_t bitCount)

#define STREAM_WRITE_BYTES_PROTOTYPE                                           \
  static inline OPSTATUS STREAM_FUNC(WriteBytes)(                              \
      STREAM_TYPE(TStream) *stream, const void *source, size_t size)

/* 8-bit operations */
#define STREAM_READ_UINT8_PROTOTYPE                                            \
  static inline OPSTATUS STREAM_FUNC(ReadUInt8)(                               \
      STREAM_TYPE(TStream) *stream, uint8_t *outValue)

#define STREAM_READ_INT8_PROTOTYPE                                             \
  static inline OPSTATUS STREAM_FUNC(ReadInt8)(                                \
      STREAM_TYPE(TStream) *stream, int8_t *outValue)

#define STREAM_PEEK_UINT8_PROTOTYPE                                            \
  static inline OPSTATUS STREAM_FUNC(PeekUInt8)(                               \
      const STREAM_TYPE(TStream) *stream, uint8_t *outValue)

#define STREAM_WRITE_UINT8_PROTOTYPE                                           \
  static inline OPSTATUS STREAM_FUNC(WriteUInt8)(                              \
      STREAM_TYPE(TStream) *stream, uint8_t value)

#define STREAM_WRITE_INT8_PROTOTYPE                                            \
  static inline OPSTATUS STREAM_FUNC(WriteInt8)(                               \
      STREAM_TYPE(TStream) *stream, int8_t value)

/* Generative prototypes for multi-byte scalar integers */
#define STREAM_READ_UINT_LE_PROTOTYPE(UTYPE, SUFFIX)                           \
  static inline OPSTATUS STREAM_FUNC(PP_OP_CAT3(ReadUInt, SUFFIX, LE))(       \
      STREAM_TYPE(TStream) *stream, UTYPE *outValue)

#define STREAM_READ_UINT_BE_PROTOTYPE(UTYPE, SUFFIX)                           \
  static inline OPSTATUS STREAM_FUNC(PP_OP_CAT3(ReadUInt, SUFFIX, BE))(       \
      STREAM_TYPE(TStream) *stream, UTYPE *outValue)

#define STREAM_READ_INT_LE_PROTOTYPE(ITYPE, SUFFIX)                            \
  static inline OPSTATUS STREAM_FUNC(PP_OP_CAT3(ReadInt, SUFFIX, LE))(        \
      STREAM_TYPE(TStream) *stream, ITYPE *outValue)

#define STREAM_READ_INT_BE_PROTOTYPE(ITYPE, SUFFIX)                            \
  static inline OPSTATUS STREAM_FUNC(PP_OP_CAT3(ReadInt, SUFFIX, BE))(        \
      STREAM_TYPE(TStream) *stream, ITYPE *outValue)

#define STREAM_WRITE_UINT_LE_PROTOTYPE(UTYPE, SUFFIX)                          \
  static inline OPSTATUS STREAM_FUNC(PP_OP_CAT3(WriteUInt, SUFFIX, LE))(      \
      STREAM_TYPE(TStream) *stream, UTYPE value)

#define STREAM_WRITE_UINT_BE_PROTOTYPE(UTYPE, SUFFIX)                          \
  static inline OPSTATUS STREAM_FUNC(PP_OP_CAT3(WriteUInt, SUFFIX, BE))(      \
      STREAM_TYPE(TStream) *stream, UTYPE value)

#define STREAM_WRITE_INT_LE_PROTOTYPE(ITYPE, SUFFIX)                           \
  static inline OPSTATUS STREAM_FUNC(PP_OP_CAT3(WriteInt, SUFFIX, LE))(       \
      STREAM_TYPE(TStream) *stream, ITYPE value)

#define STREAM_WRITE_INT_BE_PROTOTYPE(ITYPE, SUFFIX)                           \
  static inline OPSTATUS STREAM_FUNC(PP_OP_CAT3(WriteInt, SUFFIX, BE))(       \
      STREAM_TYPE(TStream) *stream, ITYPE value)

#define STREAM_SCALAR_PROTOTYPE_ALL(UTYPE, ITYPE, SUFFIX)                      \
  STREAM_READ_UINT_LE_PROTOTYPE(UTYPE, SUFFIX);                                \
  STREAM_READ_UINT_BE_PROTOTYPE(UTYPE, SUFFIX);                                \
  STREAM_READ_INT_LE_PROTOTYPE(ITYPE, SUFFIX);                                 \
  STREAM_READ_INT_BE_PROTOTYPE(ITYPE, SUFFIX);                                 \
  STREAM_WRITE_UINT_LE_PROTOTYPE(UTYPE, SUFFIX);                               \
  STREAM_WRITE_UINT_BE_PROTOTYPE(UTYPE, SUFFIX);                               \
  STREAM_WRITE_INT_LE_PROTOTYPE(ITYPE, SUFFIX);                                \
  STREAM_WRITE_INT_BE_PROTOTYPE(ITYPE, SUFFIX);

/* Prototypes declarations */
STREAM_INIT_READ_PROTOTYPE;
STREAM_INIT_WRITE_PROTOTYPE;
STREAM_REMAINING_PROTOTYPE;
STREAM_REMAINING_BITS_PROTOTYPE;
STREAM_POSITION_PROTOTYPE;
STREAM_BIT_POSITION_PROTOTYPE;
STREAM_CAPACITY_PROTOTYPE;
STREAM_IS_AT_END_PROTOTYPE;
STREAM_IS_READ_ONLY_PROTOTYPE;
STREAM_HAS_REMAINING_PROTOTYPE;
STREAM_HAS_REMAINING_BITS_PROTOTYPE;
STREAM_DATA_PROTOTYPE;
STREAM_CURRENT_PROTOTYPE;
STREAM_SEEK_PROTOTYPE;
STREAM_SKIP_PROTOTYPE;
STREAM_REWIND_PROTOTYPE;
STREAM_ALIGN_TO_BYTE_PROTOTYPE;
STREAM_SLICE_PROTOTYPE;
STREAM_READ_SLICE_PROTOTYPE;
STREAM_READ_BYTES_PROTOTYPE;
STREAM_PEEK_BYTES_PROTOTYPE;
STREAM_READ_BITS_PROTOTYPE;
STREAM_WRITE_BITS_PROTOTYPE;
STREAM_WRITE_BYTES_PROTOTYPE;
STREAM_READ_UINT8_PROTOTYPE;
STREAM_READ_INT8_PROTOTYPE;
STREAM_PEEK_UINT8_PROTOTYPE;
STREAM_WRITE_UINT8_PROTOTYPE;
STREAM_WRITE_INT8_PROTOTYPE;

STREAM_SCALAR_PROTOTYPE_ALL(uint16_t, int16_t, 16)
STREAM_SCALAR_PROTOTYPE_ALL(uint32_t, int32_t, 32)
STREAM_SCALAR_PROTOTYPE_ALL(uint64_t, int64_t, 64)

#include "Impl/Stream.impl"
/* EOF */
