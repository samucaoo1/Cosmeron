#include "../../Cosmeron/Modules/Parsing/Parsing.h"

#include <stdint.h>
#include <string.h>

#define CHECK_TRUE(COND)                                                       \
  do {                                                                         \
    if (!(COND))                                                               \
      return __LINE__;                                                         \
  } while (0)

#define CHECK_STATUS(EXPR, EXPECTED)                                           \
  do {                                                                         \
    if ((EXPR) != (EXPECTED))                                                  \
      return __LINE__;                                                         \
  } while (0)

static int Test_InitAndProperties(void) {
  const uint8_t sample[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  uint8_t buffer[16] = {0};
  PARSING_TYPE(TCursor) cursor;

  CHECK_STATUS(PARSING_FUNC(Cursor_InitRead)(NULL, sample, sizeof(sample)),
               STATUS_CONST(INVALID_ARGUMENT));
  CHECK_STATUS(PARSING_FUNC(Cursor_InitRead)(&cursor, NULL, 5),
               STATUS_CONST(INVALID_ARGUMENT));
  CHECK_STATUS(PARSING_FUNC(Cursor_InitRead)(&cursor, NULL, 0),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(PARSING_FUNC(Cursor_Capacity)(&cursor) == 0);
  CHECK_TRUE(PARSING_FUNC(Cursor_Remaining)(&cursor) == 0);
  CHECK_TRUE(PARSING_FUNC(Cursor_RemainingBits)(&cursor) == 0);
  CHECK_TRUE(PARSING_FUNC(Cursor_IsAtEnd)(&cursor));

  CHECK_STATUS(PARSING_FUNC(Cursor_InitRead)(&cursor, sample, sizeof(sample)),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(PARSING_FUNC(Cursor_Capacity)(&cursor) == 8);
  CHECK_TRUE(PARSING_FUNC(Cursor_Remaining)(&cursor) == 8);
  CHECK_TRUE(PARSING_FUNC(Cursor_RemainingBits)(&cursor) == 64);
  CHECK_TRUE(PARSING_FUNC(Cursor_Position)(&cursor) == 0);
  CHECK_TRUE(PARSING_FUNC(Cursor_BitPosition)(&cursor) == 0);
  CHECK_TRUE(!PARSING_FUNC(Cursor_IsAtEnd)(&cursor));
  CHECK_TRUE(PARSING_FUNC(Cursor_IsReadOnly)(&cursor));
  CHECK_TRUE(PARSING_FUNC(Cursor_Data)(&cursor) == (const void *)sample);
  CHECK_TRUE(PARSING_FUNC(Cursor_Current)(&cursor) == (const void *)sample);
  CHECK_TRUE(PARSING_FUNC(Cursor_HasRemaining)(&cursor, 8));
  CHECK_TRUE(!PARSING_FUNC(Cursor_HasRemaining)(&cursor, 9));
  CHECK_TRUE(PARSING_FUNC(Cursor_HasRemainingBits)(&cursor, 64));
  CHECK_TRUE(!PARSING_FUNC(Cursor_HasRemainingBits)(&cursor, 65));

  CHECK_STATUS(PARSING_FUNC(Cursor_InitWrite)(NULL, buffer, sizeof(buffer)),
               STATUS_CONST(INVALID_ARGUMENT));
  CHECK_STATUS(PARSING_FUNC(Cursor_InitWrite)(&cursor, NULL, 4),
               STATUS_CONST(INVALID_ARGUMENT));
  CHECK_STATUS(PARSING_FUNC(Cursor_InitWrite)(&cursor, buffer, sizeof(buffer)),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(PARSING_FUNC(Cursor_Capacity)(&cursor) == 16);
  CHECK_TRUE(PARSING_FUNC(Cursor_Remaining)(&cursor) == 16);
  CHECK_TRUE(!PARSING_FUNC(Cursor_IsReadOnly)(&cursor));

  return 0;
}

static int Test_SeekAndNavigation(void) {
  const uint8_t sample[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  PARSING_TYPE(TCursor) cursor;

  CHECK_STATUS(PARSING_FUNC(Cursor_InitRead)(&cursor, sample, sizeof(sample)),
               STATUS_CONST(SUCCESS));

  /* SEEK_BEGIN */
  CHECK_STATUS(
      PARSING_FUNC(Cursor_Seek)(&cursor, 4, PARSING_CONST(CURSOR_SEEK_BEGIN)),
      STATUS_CONST(SUCCESS));
  CHECK_TRUE(PARSING_FUNC(Cursor_Position)(&cursor) == 4);
  CHECK_TRUE(PARSING_FUNC(Cursor_Remaining)(&cursor) == 6);

  CHECK_STATUS(
      PARSING_FUNC(Cursor_Seek)(&cursor, -1, PARSING_CONST(CURSOR_SEEK_BEGIN)),
      STATUS_CONST(OUT_OF_RANGE));
  CHECK_STATUS(
      PARSING_FUNC(Cursor_Seek)(&cursor, 11, PARSING_CONST(CURSOR_SEEK_BEGIN)),
      STATUS_CONST(OUT_OF_RANGE));

  /* SEEK_CURRENT */
  CHECK_STATUS(
      PARSING_FUNC(Cursor_Seek)(&cursor, 3, PARSING_CONST(CURSOR_SEEK_CURRENT)),
      STATUS_CONST(SUCCESS));
  CHECK_TRUE(PARSING_FUNC(Cursor_Position)(&cursor) == 7);

  CHECK_STATUS(
      PARSING_FUNC(Cursor_Seek)(&cursor, -2, PARSING_CONST(CURSOR_SEEK_CURRENT)),
      STATUS_CONST(SUCCESS));
  CHECK_TRUE(PARSING_FUNC(Cursor_Position)(&cursor) == 5);

  CHECK_STATUS(
      PARSING_FUNC(Cursor_Seek)(&cursor, -6, PARSING_CONST(CURSOR_SEEK_CURRENT)),
      STATUS_CONST(OUT_OF_RANGE));
  CHECK_STATUS(
      PARSING_FUNC(Cursor_Seek)(&cursor, 6, PARSING_CONST(CURSOR_SEEK_CURRENT)),
      STATUS_CONST(OUT_OF_RANGE));

  /* SEEK_END */
  CHECK_STATUS(
      PARSING_FUNC(Cursor_Seek)(&cursor, 0, PARSING_CONST(CURSOR_SEEK_END)),
      STATUS_CONST(SUCCESS));
  CHECK_TRUE(PARSING_FUNC(Cursor_Position)(&cursor) == 10);
  CHECK_TRUE(PARSING_FUNC(Cursor_IsAtEnd)(&cursor));

  CHECK_STATUS(
      PARSING_FUNC(Cursor_Seek)(&cursor, -4, PARSING_CONST(CURSOR_SEEK_END)),
      STATUS_CONST(SUCCESS));
  CHECK_TRUE(PARSING_FUNC(Cursor_Position)(&cursor) == 6);

  CHECK_STATUS(
      PARSING_FUNC(Cursor_Seek)(&cursor, 1, PARSING_CONST(CURSOR_SEEK_END)),
      STATUS_CONST(OUT_OF_RANGE));
  CHECK_STATUS(
      PARSING_FUNC(Cursor_Seek)(&cursor, -11, PARSING_CONST(CURSOR_SEEK_END)),
      STATUS_CONST(OUT_OF_RANGE));

  /* Skip and Rewind */
  CHECK_STATUS(PARSING_FUNC(Cursor_Rewind)(&cursor), STATUS_CONST(SUCCESS));
  CHECK_TRUE(PARSING_FUNC(Cursor_Position)(&cursor) == 0);

  CHECK_STATUS(PARSING_FUNC(Cursor_Skip)(&cursor, 4), STATUS_CONST(SUCCESS));
  CHECK_TRUE(PARSING_FUNC(Cursor_Position)(&cursor) == 4);

  CHECK_STATUS(PARSING_FUNC(Cursor_Skip)(&cursor, 7), STATUS_CONST(OUT_OF_RANGE));
  CHECK_TRUE(PARSING_FUNC(Cursor_Position)(&cursor) == 4);

  return 0;
}

static int Test_Slicing(void) {
  const uint8_t sample[10] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
  PARSING_TYPE(TCursor) cursor;
  PARSING_TYPE(TCursor) sub;

  CHECK_STATUS(PARSING_FUNC(Cursor_InitRead)(&cursor, sample, sizeof(sample)),
               STATUS_CONST(SUCCESS));
  CHECK_STATUS(PARSING_FUNC(Cursor_Skip)(&cursor, 2), STATUS_CONST(SUCCESS));

  /* Non-advancing slice */
  CHECK_STATUS(PARSING_FUNC(Cursor_Slice)(&cursor, 4, &sub),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(PARSING_FUNC(Cursor_Position)(&cursor) == 2);
  CHECK_TRUE(PARSING_FUNC(Cursor_Capacity)(&sub) == 4);
  CHECK_TRUE(PARSING_FUNC(Cursor_Remaining)(&sub) == 4);
  CHECK_TRUE(PARSING_FUNC(Cursor_IsReadOnly)(&sub));

  uint8_t temp[4] = {0};
  CHECK_STATUS(PARSING_FUNC(Cursor_ReadBytes)(&sub, temp, 4),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(temp[0] == 30 && temp[1] == 40 && temp[2] == 50 && temp[3] == 60);

  /* Advancing slice */
  CHECK_STATUS(PARSING_FUNC(Cursor_ReadSlice)(&cursor, 3, &sub),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(PARSING_FUNC(Cursor_Position)(&cursor) == 5);
  CHECK_TRUE(PARSING_FUNC(Cursor_Capacity)(&sub) == 3);

  CHECK_STATUS(PARSING_FUNC(Cursor_ReadBytes)(&sub, temp, 3),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(temp[0] == 30 && temp[1] == 40 && temp[2] == 50);

  /* Slice overflow */
  CHECK_STATUS(PARSING_FUNC(Cursor_Slice)(&cursor, 10, &sub),
               STATUS_CONST(OUT_OF_RANGE));

  return 0;
}

static int Test_ReadingScalars(void) {
  const uint8_t wire[30] = {
      0x42,
      0xFB,
      0x34, 0x12,
      0x12, 0x34,
      0xEF, 0xBE, 0xAD, 0xDE,
      0xCA, 0xFE, 0xBA, 0xBE,
      0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01,
      0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
  };

  PARSING_TYPE(TCursor) cursor;
  uint8_t u8Val = 0;
  int8_t i8Val = 0;
  uint16_t u16Val = 0;
  uint32_t u32Val = 0;
  uint64_t u64Val = 0;

  CHECK_STATUS(PARSING_FUNC(Cursor_InitRead)(&cursor, wire, sizeof(wire)),
               STATUS_CONST(SUCCESS));

  /* Peek u8 */
  CHECK_STATUS(PARSING_FUNC(Cursor_PeekUInt8)(&cursor, &u8Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u8Val == 0x42);
  CHECK_TRUE(PARSING_FUNC(Cursor_Position)(&cursor) == 0);

  /* Read u8 & i8 */
  CHECK_STATUS(PARSING_FUNC(Cursor_ReadUInt8)(&cursor, &u8Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u8Val == 0x42);

  CHECK_STATUS(PARSING_FUNC(Cursor_ReadInt8)(&cursor, &i8Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i8Val == -5);

  /* Read u16 */
  CHECK_STATUS(PARSING_FUNC(Cursor_ReadUInt16LE)(&cursor, &u16Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u16Val == 0x1234U);

  CHECK_STATUS(PARSING_FUNC(Cursor_ReadUInt16BE)(&cursor, &u16Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u16Val == 0x1234U);

  /* Read u32 */
  CHECK_STATUS(PARSING_FUNC(Cursor_ReadUInt32LE)(&cursor, &u32Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u32Val == 0xDEADBEEFU);

  CHECK_STATUS(PARSING_FUNC(Cursor_ReadUInt32BE)(&cursor, &u32Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u32Val == 0xCAFEBABEU);

  /* Read u64 */
  CHECK_STATUS(PARSING_FUNC(Cursor_ReadUInt64LE)(&cursor, &u64Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u64Val == 0x0102030405060708ULL);

  CHECK_STATUS(PARSING_FUNC(Cursor_ReadUInt64BE)(&cursor, &u64Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u64Val == 0x0102030405060708ULL);

  CHECK_TRUE(PARSING_FUNC(Cursor_IsAtEnd)(&cursor));
  CHECK_STATUS(PARSING_FUNC(Cursor_ReadUInt8)(&cursor, &u8Val),
               STATUS_CONST(OUT_OF_RANGE));

  return 0;
}

static int Test_WritingAndRoundtrip(void) {
  uint8_t buffer[64] = {0};
  const uint8_t roBuffer[8] = {0};
  PARSING_TYPE(TCursor) cursor;

  /* Writing to read-only cursor fails */
  CHECK_STATUS(PARSING_FUNC(Cursor_InitRead)(&cursor, roBuffer, sizeof(roBuffer)),
               STATUS_CONST(SUCCESS));
  CHECK_STATUS(PARSING_FUNC(Cursor_WriteUInt8)(&cursor, 10),
               STATUS_CONST(NOT_SUPPORTED));
  CHECK_STATUS(PARSING_FUNC(Cursor_WriteBytes)(&cursor, "abc", 3),
               STATUS_CONST(NOT_SUPPORTED));

  /* Write and read back */
  CHECK_STATUS(PARSING_FUNC(Cursor_InitWrite)(&cursor, buffer, sizeof(buffer)),
               STATUS_CONST(SUCCESS));

  CHECK_STATUS(PARSING_FUNC(Cursor_WriteUInt8)(&cursor, 0xAAU),
               STATUS_CONST(SUCCESS));
  CHECK_STATUS(PARSING_FUNC(Cursor_WriteInt8)(&cursor, -12),
               STATUS_CONST(SUCCESS));
  CHECK_STATUS(PARSING_FUNC(Cursor_WriteUInt16LE)(&cursor, 0x55AAU),
               STATUS_CONST(SUCCESS));
  CHECK_STATUS(PARSING_FUNC(Cursor_WriteUInt16BE)(&cursor, 0x1234U),
               STATUS_CONST(SUCCESS));
  CHECK_STATUS(PARSING_FUNC(Cursor_WriteUInt32LE)(&cursor, 0x87654321U),
               STATUS_CONST(SUCCESS));
  CHECK_STATUS(PARSING_FUNC(Cursor_WriteUInt32BE)(&cursor, 0x11223344U),
               STATUS_CONST(SUCCESS));
  CHECK_STATUS(PARSING_FUNC(Cursor_WriteUInt64LE)(&cursor, 0xA1B2C3D4E5F60718ULL),
               STATUS_CONST(SUCCESS));
  CHECK_STATUS(PARSING_FUNC(Cursor_WriteUInt64BE)(&cursor, 0xFEDCBA9876543210ULL),
               STATUS_CONST(SUCCESS));

  const char *tag = "COSMERON";
  CHECK_STATUS(PARSING_FUNC(Cursor_WriteBytes)(&cursor, tag, 8),
               STATUS_CONST(SUCCESS));

  size_t writtenBytes = PARSING_FUNC(Cursor_Position)(&cursor);
  CHECK_TRUE(writtenBytes == (1 + 1 + 2 + 2 + 4 + 4 + 8 + 8 + 8));

  /* Read-back verification */
  PARSING_TYPE(TCursor) reader;
  CHECK_STATUS(PARSING_FUNC(Cursor_InitRead)(&reader, buffer, writtenBytes),
               STATUS_CONST(SUCCESS));

  uint8_t u8Val = 0;
  int8_t i8Val = 0;
  uint16_t u16Val = 0;
  uint32_t u32Val = 0;
  uint64_t u64Val = 0;
  char tagRead[9] = {0};

  CHECK_STATUS(PARSING_FUNC(Cursor_ReadUInt8)(&reader, &u8Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u8Val == 0xAAU);

  CHECK_STATUS(PARSING_FUNC(Cursor_ReadInt8)(&reader, &i8Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i8Val == -12);

  CHECK_STATUS(PARSING_FUNC(Cursor_ReadUInt16LE)(&reader, &u16Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u16Val == 0x55AAU);

  CHECK_STATUS(PARSING_FUNC(Cursor_ReadUInt16BE)(&reader, &u16Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u16Val == 0x1234U);

  CHECK_STATUS(PARSING_FUNC(Cursor_ReadUInt32LE)(&reader, &u32Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u32Val == 0x87654321U);

  CHECK_STATUS(PARSING_FUNC(Cursor_ReadUInt32BE)(&reader, &u32Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u32Val == 0x11223344U);

  CHECK_STATUS(PARSING_FUNC(Cursor_ReadUInt64LE)(&reader, &u64Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u64Val == 0xA1B2C3D4E5F60718ULL);

  CHECK_STATUS(PARSING_FUNC(Cursor_ReadUInt64BE)(&reader, &u64Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u64Val == 0xFEDCBA9876543210ULL);

  CHECK_STATUS(PARSING_FUNC(Cursor_ReadBytes)(&reader, tagRead, 8),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(memcmp(tagRead, "COSMERON", 8) == 0);
  CHECK_TRUE(PARSING_FUNC(Cursor_IsAtEnd)(&reader));

  return 0;
}

static int Test_BitLevelOperations(void) {
  uint8_t buffer[8] = {0};
  PARSING_TYPE(TCursor) cursor;

  CHECK_STATUS(PARSING_FUNC(Cursor_InitWrite)(&cursor, buffer, sizeof(buffer)),
               STATUS_CONST(SUCCESS));

  /* Write 3 bits: 5 (binary 101) */
  CHECK_STATUS(PARSING_FUNC(Cursor_WriteBits)(&cursor, 5, 3),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(PARSING_FUNC(Cursor_Position)(&cursor) == 0);
  CHECK_TRUE(PARSING_FUNC(Cursor_BitPosition)(&cursor) == 3);

  /* Write 5 bits: 18 (binary 10010) -> completes first byte: 10110010 (0xB2) */
  CHECK_STATUS(PARSING_FUNC(Cursor_WriteBits)(&cursor, 18, 5),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(PARSING_FUNC(Cursor_Position)(&cursor) == 1);
  CHECK_TRUE(PARSING_FUNC(Cursor_BitPosition)(&cursor) == 0);
  CHECK_TRUE(buffer[0] == 0xB2U);

  /* Write 12 bits: 0xABC (binary 1010 1011 1100) */
  CHECK_STATUS(PARSING_FUNC(Cursor_WriteBits)(&cursor, 0xABCU, 12),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(PARSING_FUNC(Cursor_Position)(&cursor) == 2);
  CHECK_TRUE(PARSING_FUNC(Cursor_BitPosition)(&cursor) == 4);

  /* Align to byte boundary */
  CHECK_STATUS(PARSING_FUNC(Cursor_AlignToByte)(&cursor), STATUS_CONST(SUCCESS));
  CHECK_TRUE(PARSING_FUNC(Cursor_Position)(&cursor) == 3);
  CHECK_TRUE(PARSING_FUNC(Cursor_BitPosition)(&cursor) == 0);

  /* Read-back bit-level verification */
  PARSING_TYPE(TCursor) reader;
  CHECK_STATUS(PARSING_FUNC(Cursor_InitRead)(&reader, buffer, 3),
               STATUS_CONST(SUCCESS));

  uint64_t val = 0;
  CHECK_STATUS(PARSING_FUNC(Cursor_ReadBits)(&reader, 3, &val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(val == 5U);

  CHECK_STATUS(PARSING_FUNC(Cursor_ReadBits)(&reader, 5, &val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(val == 18U);

  CHECK_STATUS(PARSING_FUNC(Cursor_ReadBits)(&reader, 12, &val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(val == 0xABCU);

  CHECK_STATUS(PARSING_FUNC(Cursor_AlignToByte)(&reader), STATUS_CONST(SUCCESS));
  CHECK_TRUE(PARSING_FUNC(Cursor_IsAtEnd)(&reader));

  return 0;
}

int main(void) {
  int result;

  result = Test_InitAndProperties();
  if (result != 0)
    return result;

  result = Test_SeekAndNavigation();
  if (result != 0)
    return result;

  result = Test_Slicing();
  if (result != 0)
    return result;

  result = Test_ReadingScalars();
  if (result != 0)
    return result;

  result = Test_WritingAndRoundtrip();
  if (result != 0)
    return result;

  result = Test_BitLevelOperations();
  if (result != 0)
    return result;

  return 0;
}
