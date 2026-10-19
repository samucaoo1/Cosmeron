#pragma once

#include "../Parsing.space"
#include "../../Type/Fundamental.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef enum PARSING_TYPE(CursorSeekOrigin) {
  PARSING_CONST(CURSOR_SEEK_BEGIN),
  PARSING_CONST(CURSOR_SEEK_CURRENT),
  PARSING_CONST(CURSOR_SEEK_END)
} PARSING_TYPE(CursorSeekOrigin);

typedef struct PARSING_TYPE(TCursor) {
  uint8_t *data;
  size_t capacity;
  size_t offset;
  uint8_t bitOffset;
  bool isReadOnly;
} PARSING_TYPE(TCursor);

#define PARSING_CURSOR_INIT_READ_PROTOTYPE                                     \
  static inline OPSTATUS PARSING_FUNC(Cursor_InitRead)(                        \
      PARSING_TYPE(TCursor) *cursor, const void *buffer, size_t size)

#define PARSING_CURSOR_INIT_WRITE_PROTOTYPE                                    \
  static inline OPSTATUS PARSING_FUNC(Cursor_InitWrite)(                       \
      PARSING_TYPE(TCursor) *cursor, void *buffer, size_t capacity)

#define PARSING_CURSOR_REMAINING_PROTOTYPE                                     \
  static inline size_t PARSING_FUNC(Cursor_Remaining)(                         \
      const PARSING_TYPE(TCursor) *cursor)

#define PARSING_CURSOR_REMAINING_BITS_PROTOTYPE                                \
  static inline size_t PARSING_FUNC(Cursor_RemainingBits)(                     \
      const PARSING_TYPE(TCursor) *cursor)

#define PARSING_CURSOR_POSITION_PROTOTYPE                                      \
  static inline size_t PARSING_FUNC(Cursor_Position)(                          \
      const PARSING_TYPE(TCursor) *cursor)

#define PARSING_CURSOR_BIT_POSITION_PROTOTYPE                                  \
  static inline uint8_t PARSING_FUNC(Cursor_BitPosition)(                      \
      const PARSING_TYPE(TCursor) *cursor)

#define PARSING_CURSOR_CAPACITY_PROTOTYPE                                      \
  static inline size_t PARSING_FUNC(Cursor_Capacity)(                          \
      const PARSING_TYPE(TCursor) *cursor)

#define PARSING_CURSOR_IS_AT_END_PROTOTYPE                                     \
  static inline bool PARSING_FUNC(Cursor_IsAtEnd)(                             \
      const PARSING_TYPE(TCursor) *cursor)

#define PARSING_CURSOR_IS_READ_ONLY_PROTOTYPE                                  \
  static inline bool PARSING_FUNC(Cursor_IsReadOnly)(                          \
      const PARSING_TYPE(TCursor) *cursor)

#define PARSING_CURSOR_HAS_REMAINING_PROTOTYPE                                 \
  static inline bool PARSING_FUNC(Cursor_HasRemaining)(                        \
      const PARSING_TYPE(TCursor) *cursor, size_t count)

#define PARSING_CURSOR_HAS_REMAINING_BITS_PROTOTYPE                            \
  static inline bool PARSING_FUNC(Cursor_HasRemainingBits)(                    \
      const PARSING_TYPE(TCursor) *cursor, size_t bitCount)

#define PARSING_CURSOR_DATA_PROTOTYPE                                          \
  static inline const void *PARSING_FUNC(Cursor_Data)(                         \
      const PARSING_TYPE(TCursor) *cursor)

#define PARSING_CURSOR_CURRENT_PROTOTYPE                                       \
  static inline const void *PARSING_FUNC(Cursor_Current)(                      \
      const PARSING_TYPE(TCursor) *cursor)

#define PARSING_CURSOR_SEEK_PROTOTYPE                                          \
  static inline OPSTATUS PARSING_FUNC(Cursor_Seek)(                            \
      PARSING_TYPE(TCursor) *cursor, int64_t offset,                           \
      PARSING_TYPE(CursorSeekOrigin) origin)

#define PARSING_CURSOR_SKIP_PROTOTYPE                                          \
  static inline OPSTATUS PARSING_FUNC(Cursor_Skip)(                            \
      PARSING_TYPE(TCursor) *cursor, size_t count)

#define PARSING_CURSOR_REWIND_PROTOTYPE                                        \
  static inline OPSTATUS PARSING_FUNC(Cursor_Rewind)(                          \
      PARSING_TYPE(TCursor) *cursor)

#define PARSING_CURSOR_ALIGN_TO_BYTE_PROTOTYPE                                 \
  static inline OPSTATUS PARSING_FUNC(Cursor_AlignToByte)(                     \
      PARSING_TYPE(TCursor) *cursor)

#define PARSING_CURSOR_SLICE_PROTOTYPE                                         \
  static inline OPSTATUS PARSING_FUNC(Cursor_Slice)(                           \
      const PARSING_TYPE(TCursor) *cursor, size_t size,                        \
      PARSING_TYPE(TCursor) *outSubCursor)

#define PARSING_CURSOR_READ_SLICE_PROTOTYPE                                    \
  static inline OPSTATUS PARSING_FUNC(Cursor_ReadSlice)(                       \
      PARSING_TYPE(TCursor) *cursor, size_t size,                              \
      PARSING_TYPE(TCursor) *outSubCursor)

#define PARSING_CURSOR_READ_BYTES_PROTOTYPE                                    \
  static inline OPSTATUS PARSING_FUNC(Cursor_ReadBytes)(                       \
      PARSING_TYPE(TCursor) *cursor, void *destination, size_t size)

#define PARSING_CURSOR_PEEK_BYTES_PROTOTYPE                                    \
  static inline OPSTATUS PARSING_FUNC(Cursor_PeekBytes)(                       \
      const PARSING_TYPE(TCursor) *cursor, void *destination, size_t size)

#define PARSING_CURSOR_READ_BITS_PROTOTYPE                                     \
  static inline OPSTATUS PARSING_FUNC(Cursor_ReadBits)(                        \
      PARSING_TYPE(TCursor) *cursor, uint8_t bitCount, uint64_t *outValue)

#define PARSING_CURSOR_WRITE_BITS_PROTOTYPE                                    \
  static inline OPSTATUS PARSING_FUNC(Cursor_WriteBits)(                       \
      PARSING_TYPE(TCursor) *cursor, uint64_t value, uint8_t bitCount)

#define PARSING_CURSOR_WRITE_BYTES_PROTOTYPE                                   \
  static inline OPSTATUS PARSING_FUNC(Cursor_WriteBytes)(                      \
      PARSING_TYPE(TCursor) *cursor, const void *source, size_t size)

/* 8-bit operations */
#define PARSING_CURSOR_READ_UINT8_PROTOTYPE                                    \
  static inline OPSTATUS PARSING_FUNC(Cursor_ReadUInt8)(                       \
      PARSING_TYPE(TCursor) *cursor, uint8_t *outValue)

#define PARSING_CURSOR_READ_INT8_PROTOTYPE                                     \
  static inline OPSTATUS PARSING_FUNC(Cursor_ReadInt8)(                        \
      PARSING_TYPE(TCursor) *cursor, int8_t *outValue)

#define PARSING_CURSOR_PEEK_UINT8_PROTOTYPE                                    \
  static inline OPSTATUS PARSING_FUNC(Cursor_PeekUInt8)(                       \
      const PARSING_TYPE(TCursor) *cursor, uint8_t *outValue)

#define PARSING_CURSOR_WRITE_UINT8_PROTOTYPE                                   \
  static inline OPSTATUS PARSING_FUNC(Cursor_WriteUInt8)(                      \
      PARSING_TYPE(TCursor) *cursor, uint8_t value)

#define PARSING_CURSOR_WRITE_INT8_PROTOTYPE                                    \
  static inline OPSTATUS PARSING_FUNC(Cursor_WriteInt8)(                       \
      PARSING_TYPE(TCursor) *cursor, int8_t value)

/* Generative prototypes for multi-byte scalar integers */
#define PARSING_CURSOR_READ_UINT_LE_PROTOTYPE(UTYPE, SUFFIX)                   \
  static inline OPSTATUS PARSING_FUNC(PP_OP_CAT3(Cursor_ReadUInt, SUFFIX, LE))(\
      PARSING_TYPE(TCursor) *cursor, UTYPE *outValue)

#define PARSING_CURSOR_READ_UINT_BE_PROTOTYPE(UTYPE, SUFFIX)                   \
  static inline OPSTATUS PARSING_FUNC(PP_OP_CAT3(Cursor_ReadUInt, SUFFIX, BE))(\
      PARSING_TYPE(TCursor) *cursor, UTYPE *outValue)

#define PARSING_CURSOR_READ_INT_LE_PROTOTYPE(ITYPE, SUFFIX)                    \
  static inline OPSTATUS PARSING_FUNC(PP_OP_CAT3(Cursor_ReadInt, SUFFIX, LE))( \
      PARSING_TYPE(TCursor) *cursor, ITYPE *outValue)

#define PARSING_CURSOR_READ_INT_BE_PROTOTYPE(ITYPE, SUFFIX)                    \
  static inline OPSTATUS PARSING_FUNC(PP_OP_CAT3(Cursor_ReadInt, SUFFIX, BE))( \
      PARSING_TYPE(TCursor) *cursor, ITYPE *outValue)

#define PARSING_CURSOR_WRITE_UINT_LE_PROTOTYPE(UTYPE, SUFFIX)                  \
  static inline OPSTATUS PARSING_FUNC(PP_OP_CAT3(Cursor_WriteUInt, SUFFIX, LE))(\
      PARSING_TYPE(TCursor) *cursor, UTYPE value)

#define PARSING_CURSOR_WRITE_UINT_BE_PROTOTYPE(UTYPE, SUFFIX)                  \
  static inline OPSTATUS PARSING_FUNC(PP_OP_CAT3(Cursor_WriteUInt, SUFFIX, BE))(\
      PARSING_TYPE(TCursor) *cursor, UTYPE value)

#define PARSING_CURSOR_WRITE_INT_LE_PROTOTYPE(ITYPE, SUFFIX)                   \
  static inline OPSTATUS PARSING_FUNC(PP_OP_CAT3(Cursor_WriteInt, SUFFIX, LE))(\
      PARSING_TYPE(TCursor) *cursor, ITYPE value)

#define PARSING_CURSOR_WRITE_INT_BE_PROTOTYPE(ITYPE, SUFFIX)                   \
  static inline OPSTATUS PARSING_FUNC(PP_OP_CAT3(Cursor_WriteInt, SUFFIX, BE))(\
      PARSING_TYPE(TCursor) *cursor, ITYPE value)

#define PARSING_CURSOR_SCALAR_PROTOTYPE_ALL(UTYPE, ITYPE, SUFFIX)              \
  PARSING_CURSOR_READ_UINT_LE_PROTOTYPE(UTYPE, SUFFIX);                        \
  PARSING_CURSOR_READ_UINT_BE_PROTOTYPE(UTYPE, SUFFIX);                        \
  PARSING_CURSOR_READ_INT_LE_PROTOTYPE(ITYPE, SUFFIX);                         \
  PARSING_CURSOR_READ_INT_BE_PROTOTYPE(ITYPE, SUFFIX);                         \
  PARSING_CURSOR_WRITE_UINT_LE_PROTOTYPE(UTYPE, SUFFIX);                       \
  PARSING_CURSOR_WRITE_UINT_BE_PROTOTYPE(UTYPE, SUFFIX);                       \
  PARSING_CURSOR_WRITE_INT_LE_PROTOTYPE(ITYPE, SUFFIX);                        \
  PARSING_CURSOR_WRITE_INT_BE_PROTOTYPE(ITYPE, SUFFIX);

PARSING_CURSOR_INIT_READ_PROTOTYPE;
PARSING_CURSOR_INIT_WRITE_PROTOTYPE;
PARSING_CURSOR_REMAINING_PROTOTYPE;
PARSING_CURSOR_REMAINING_BITS_PROTOTYPE;
PARSING_CURSOR_POSITION_PROTOTYPE;
PARSING_CURSOR_BIT_POSITION_PROTOTYPE;
PARSING_CURSOR_CAPACITY_PROTOTYPE;
PARSING_CURSOR_IS_AT_END_PROTOTYPE;
PARSING_CURSOR_IS_READ_ONLY_PROTOTYPE;
PARSING_CURSOR_HAS_REMAINING_PROTOTYPE;
PARSING_CURSOR_HAS_REMAINING_BITS_PROTOTYPE;
PARSING_CURSOR_DATA_PROTOTYPE;
PARSING_CURSOR_CURRENT_PROTOTYPE;
PARSING_CURSOR_SEEK_PROTOTYPE;
PARSING_CURSOR_SKIP_PROTOTYPE;
PARSING_CURSOR_REWIND_PROTOTYPE;
PARSING_CURSOR_ALIGN_TO_BYTE_PROTOTYPE;
PARSING_CURSOR_SLICE_PROTOTYPE;
PARSING_CURSOR_READ_SLICE_PROTOTYPE;
PARSING_CURSOR_READ_BYTES_PROTOTYPE;
PARSING_CURSOR_PEEK_BYTES_PROTOTYPE;
PARSING_CURSOR_READ_BITS_PROTOTYPE;
PARSING_CURSOR_WRITE_BITS_PROTOTYPE;
PARSING_CURSOR_WRITE_BYTES_PROTOTYPE;
PARSING_CURSOR_READ_UINT8_PROTOTYPE;
PARSING_CURSOR_READ_INT8_PROTOTYPE;
PARSING_CURSOR_PEEK_UINT8_PROTOTYPE;
PARSING_CURSOR_WRITE_UINT8_PROTOTYPE;
PARSING_CURSOR_WRITE_INT8_PROTOTYPE;

PARSING_CURSOR_SCALAR_PROTOTYPE_ALL(uint16_t, int16_t, 16)
PARSING_CURSOR_SCALAR_PROTOTYPE_ALL(uint32_t, int32_t, 32)
PARSING_CURSOR_SCALAR_PROTOTYPE_ALL(uint64_t, int64_t, 64)

#include "Impl/Cursor.impl"
/* EOF */
