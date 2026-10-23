#include "../../Cosmeron/Core/Memory/Stream.h"

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
  STREAM_TYPE(TStream) stream;

  CHECK_STATUS(STREAM_FUNC(InitRead)(NULL, sample, sizeof(sample)),
               STATUS_CONST(INVALID_ARGUMENT));
  CHECK_STATUS(STREAM_FUNC(InitRead)(&stream, NULL, 5),
               STATUS_CONST(INVALID_ARGUMENT));
  CHECK_STATUS(STREAM_FUNC(InitRead)(&stream, NULL, 0),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(STREAM_FUNC(Capacity)(&stream) == 0);
  CHECK_TRUE(STREAM_FUNC(Remaining)(&stream) == 0);
  CHECK_TRUE(STREAM_FUNC(RemainingBits)(&stream) == 0);
  CHECK_TRUE(STREAM_FUNC(IsAtEnd)(&stream));

  CHECK_STATUS(STREAM_FUNC(InitRead)(&stream, sample, sizeof(sample)),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(STREAM_FUNC(Capacity)(&stream) == 8);
  CHECK_TRUE(STREAM_FUNC(Remaining)(&stream) == 8);
  CHECK_TRUE(STREAM_FUNC(RemainingBits)(&stream) == 64);
  CHECK_TRUE(STREAM_FUNC(Position)(&stream) == 0);
  CHECK_TRUE(STREAM_FUNC(BitPosition)(&stream) == 0);
  CHECK_TRUE(!STREAM_FUNC(IsAtEnd)(&stream));
  CHECK_TRUE(STREAM_FUNC(IsReadOnly)(&stream));
  CHECK_TRUE(STREAM_FUNC(Data)(&stream) == (const void *)sample);
  CHECK_TRUE(STREAM_FUNC(Current)(&stream) == (const void *)sample);
  CHECK_TRUE(STREAM_FUNC(HasRemaining)(&stream, 8));
  CHECK_TRUE(!STREAM_FUNC(HasRemaining)(&stream, 9));
  CHECK_TRUE(STREAM_FUNC(HasRemainingBits)(&stream, 64));
  CHECK_TRUE(!STREAM_FUNC(HasRemainingBits)(&stream, 65));

  CHECK_STATUS(STREAM_FUNC(InitWrite)(NULL, buffer, sizeof(buffer)),
               STATUS_CONST(INVALID_ARGUMENT));
  CHECK_STATUS(STREAM_FUNC(InitWrite)(&stream, NULL, 4),
               STATUS_CONST(INVALID_ARGUMENT));
  CHECK_STATUS(STREAM_FUNC(InitWrite)(&stream, buffer, sizeof(buffer)),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(STREAM_FUNC(Capacity)(&stream) == 16);
  CHECK_TRUE(STREAM_FUNC(Remaining)(&stream) == 16);
  CHECK_TRUE(!STREAM_FUNC(IsReadOnly)(&stream));

  return 0;
}

static int Test_SeekAndNavigation(void) {
  const uint8_t sample[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  STREAM_TYPE(TStream) stream;

  CHECK_STATUS(STREAM_FUNC(InitRead)(&stream, sample, sizeof(sample)),
               STATUS_CONST(SUCCESS));

  /* SEEK_BEGIN */
  CHECK_STATUS(
      STREAM_FUNC(Seek)(&stream, 4, STREAM_CONST(SEEK_BEGIN)),
      STATUS_CONST(SUCCESS));
  CHECK_TRUE(STREAM_FUNC(Position)(&stream) == 4);
  CHECK_TRUE(STREAM_FUNC(Remaining)(&stream) == 6);

  CHECK_STATUS(
      STREAM_FUNC(Seek)(&stream, -1, STREAM_CONST(SEEK_BEGIN)),
      STATUS_CONST(OUT_OF_RANGE));
  CHECK_STATUS(
      STREAM_FUNC(Seek)(&stream, 11, STREAM_CONST(SEEK_BEGIN)),
      STATUS_CONST(OUT_OF_RANGE));

  /* SEEK_CURRENT */
  CHECK_STATUS(
      STREAM_FUNC(Seek)(&stream, 3, STREAM_CONST(SEEK_CURRENT)),
      STATUS_CONST(SUCCESS));
  CHECK_TRUE(STREAM_FUNC(Position)(&stream) == 7);

  CHECK_STATUS(
      STREAM_FUNC(Seek)(&stream, -2, STREAM_CONST(SEEK_CURRENT)),
      STATUS_CONST(SUCCESS));
  CHECK_TRUE(STREAM_FUNC(Position)(&stream) == 5);

  CHECK_STATUS(
      STREAM_FUNC(Seek)(&stream, -6, STREAM_CONST(SEEK_CURRENT)),
      STATUS_CONST(OUT_OF_RANGE));
  CHECK_STATUS(
      STREAM_FUNC(Seek)(&stream, 6, STREAM_CONST(SEEK_CURRENT)),
      STATUS_CONST(OUT_OF_RANGE));

  /* SEEK_END */
  CHECK_STATUS(
      STREAM_FUNC(Seek)(&stream, 0, STREAM_CONST(SEEK_END)),
      STATUS_CONST(SUCCESS));
  CHECK_TRUE(STREAM_FUNC(Position)(&stream) == 10);
  CHECK_TRUE(STREAM_FUNC(IsAtEnd)(&stream));

  CHECK_STATUS(
      STREAM_FUNC(Seek)(&stream, -4, STREAM_CONST(SEEK_END)),
      STATUS_CONST(SUCCESS));
  CHECK_TRUE(STREAM_FUNC(Position)(&stream) == 6);

  CHECK_STATUS(
      STREAM_FUNC(Seek)(&stream, 1, STREAM_CONST(SEEK_END)),
      STATUS_CONST(OUT_OF_RANGE));
  CHECK_STATUS(
      STREAM_FUNC(Seek)(&stream, -11, STREAM_CONST(SEEK_END)),
      STATUS_CONST(OUT_OF_RANGE));

  /* Skip and Rewind */
  CHECK_STATUS(STREAM_FUNC(Rewind)(&stream), STATUS_CONST(SUCCESS));
  CHECK_TRUE(STREAM_FUNC(Position)(&stream) == 0);

  CHECK_STATUS(STREAM_FUNC(Skip)(&stream, 4), STATUS_CONST(SUCCESS));
  CHECK_TRUE(STREAM_FUNC(Position)(&stream) == 4);

  CHECK_STATUS(STREAM_FUNC(Skip)(&stream, 7), STATUS_CONST(OUT_OF_RANGE));
  CHECK_TRUE(STREAM_FUNC(Position)(&stream) == 4);

  return 0;
}

static int Test_Slicing(void) {
  const uint8_t sample[10] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
  STREAM_TYPE(TStream) stream;
  STREAM_TYPE(TStream) sub;

  CHECK_STATUS(STREAM_FUNC(InitRead)(&stream, sample, sizeof(sample)),
               STATUS_CONST(SUCCESS));
  CHECK_STATUS(STREAM_FUNC(Skip)(&stream, 2), STATUS_CONST(SUCCESS));

  /* Non-advancing slice */
  CHECK_STATUS(STREAM_FUNC(Slice)(&stream, 4, &sub),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(STREAM_FUNC(Position)(&stream) == 2);
  CHECK_TRUE(STREAM_FUNC(Capacity)(&sub) == 4);
  CHECK_TRUE(STREAM_FUNC(Remaining)(&sub) == 4);
  CHECK_TRUE(STREAM_FUNC(IsReadOnly)(&sub));

  uint8_t temp[4] = {0};
  CHECK_STATUS(STREAM_FUNC(ReadBytes)(&sub, temp, 4),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(temp[0] == 30 && temp[1] == 40 && temp[2] == 50 && temp[3] == 60);

  /* Advancing slice */
  CHECK_STATUS(STREAM_FUNC(ReadSlice)(&stream, 3, &sub),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(STREAM_FUNC(Position)(&stream) == 5);
  CHECK_TRUE(STREAM_FUNC(Capacity)(&sub) == 3);

  CHECK_STATUS(STREAM_FUNC(ReadBytes)(&sub, temp, 3),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(temp[0] == 30 && temp[1] == 40 && temp[2] == 50);

  /* Slice overflow */
  CHECK_STATUS(STREAM_FUNC(Slice)(&stream, 10, &sub),
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

  STREAM_TYPE(TStream) stream;
  uint8_t u8Val = 0;
  int8_t i8Val = 0;
  uint16_t u16Val = 0;
  uint32_t u32Val = 0;
  uint64_t u64Val = 0;

  CHECK_STATUS(STREAM_FUNC(InitRead)(&stream, wire, sizeof(wire)),
               STATUS_CONST(SUCCESS));

  /* Peek u8 */
  CHECK_STATUS(STREAM_FUNC(PeekUInt8)(&stream, &u8Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u8Val == 0x42);
  CHECK_TRUE(STREAM_FUNC(Position)(&stream) == 0);

  /* Read u8 & i8 */
  CHECK_STATUS(STREAM_FUNC(ReadUInt8)(&stream, &u8Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u8Val == 0x42);

  CHECK_STATUS(STREAM_FUNC(ReadInt8)(&stream, &i8Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i8Val == -5);

  /* Read u16 */
  CHECK_STATUS(STREAM_FUNC(ReadUInt16LE)(&stream, &u16Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u16Val == 0x1234U);

  CHECK_STATUS(STREAM_FUNC(ReadUInt16BE)(&stream, &u16Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u16Val == 0x1234U);

  /* Read u32 */
  CHECK_STATUS(STREAM_FUNC(ReadUInt32LE)(&stream, &u32Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u32Val == 0xDEADBEEFU);

  CHECK_STATUS(STREAM_FUNC(ReadUInt32BE)(&stream, &u32Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u32Val == 0xCAFEBABEU);

  /* Read u64 */
  CHECK_STATUS(STREAM_FUNC(ReadUInt64LE)(&stream, &u64Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u64Val == 0x0102030405060708ULL);

  CHECK_STATUS(STREAM_FUNC(ReadUInt64BE)(&stream, &u64Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u64Val == 0x0102030405060708ULL);

  CHECK_TRUE(STREAM_FUNC(IsAtEnd)(&stream));
  CHECK_STATUS(STREAM_FUNC(ReadUInt8)(&stream, &u8Val),
               STATUS_CONST(OUT_OF_RANGE));

  return 0;
}

static int Test_WritingAndRoundtrip(void) {
  uint8_t buffer[64] = {0};
  const uint8_t roBuffer[8] = {0};
  STREAM_TYPE(TStream) stream;

  /* Writing to read-only stream fails */
  CHECK_STATUS(STREAM_FUNC(InitRead)(&stream, roBuffer, sizeof(roBuffer)),
               STATUS_CONST(SUCCESS));
  CHECK_STATUS(STREAM_FUNC(WriteUInt8)(&stream, 10),
               STATUS_CONST(NOT_SUPPORTED));
  CHECK_STATUS(STREAM_FUNC(WriteBytes)(&stream, "abc", 3),
               STATUS_CONST(NOT_SUPPORTED));

  /* Write and read back */
  CHECK_STATUS(STREAM_FUNC(InitWrite)(&stream, buffer, sizeof(buffer)),
               STATUS_CONST(SUCCESS));

  CHECK_STATUS(STREAM_FUNC(WriteUInt8)(&stream, 0xAAU),
               STATUS_CONST(SUCCESS));
  CHECK_STATUS(STREAM_FUNC(WriteInt8)(&stream, -12),
               STATUS_CONST(SUCCESS));
  CHECK_STATUS(STREAM_FUNC(WriteUInt16LE)(&stream, 0x55AAU),
               STATUS_CONST(SUCCESS));
  CHECK_STATUS(STREAM_FUNC(WriteUInt16BE)(&stream, 0x1234U),
               STATUS_CONST(SUCCESS));
  CHECK_STATUS(STREAM_FUNC(WriteUInt32LE)(&stream, 0x87654321U),
               STATUS_CONST(SUCCESS));
  CHECK_STATUS(STREAM_FUNC(WriteUInt32BE)(&stream, 0x11223344U),
               STATUS_CONST(SUCCESS));
  CHECK_STATUS(STREAM_FUNC(WriteUInt64LE)(&stream, 0xA1B2C3D4E5F60718ULL),
               STATUS_CONST(SUCCESS));
  CHECK_STATUS(STREAM_FUNC(WriteUInt64BE)(&stream, 0xFEDCBA9876543210ULL),
               STATUS_CONST(SUCCESS));

  const char *tag = "COSMERON";
  CHECK_STATUS(STREAM_FUNC(WriteBytes)(&stream, tag, 8),
               STATUS_CONST(SUCCESS));

  size_t writtenBytes = STREAM_FUNC(Position)(&stream);
  CHECK_TRUE(writtenBytes == (1 + 1 + 2 + 2 + 4 + 4 + 8 + 8 + 8));

  /* Read-back verification */
  STREAM_TYPE(TStream) reader;
  CHECK_STATUS(STREAM_FUNC(InitRead)(&reader, buffer, writtenBytes),
               STATUS_CONST(SUCCESS));

  uint8_t u8Val = 0;
  int8_t i8Val = 0;
  uint16_t u16Val = 0;
  uint32_t u32Val = 0;
  uint64_t u64Val = 0;
  char tagRead[9] = {0};

  CHECK_STATUS(STREAM_FUNC(ReadUInt8)(&reader, &u8Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u8Val == 0xAAU);

  CHECK_STATUS(STREAM_FUNC(ReadInt8)(&reader, &i8Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i8Val == -12);

  CHECK_STATUS(STREAM_FUNC(ReadUInt16LE)(&reader, &u16Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u16Val == 0x55AAU);

  CHECK_STATUS(STREAM_FUNC(ReadUInt16BE)(&reader, &u16Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u16Val == 0x1234U);

  CHECK_STATUS(STREAM_FUNC(ReadUInt32LE)(&reader, &u32Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u32Val == 0x87654321U);

  CHECK_STATUS(STREAM_FUNC(ReadUInt32BE)(&reader, &u32Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u32Val == 0x11223344U);

  CHECK_STATUS(STREAM_FUNC(ReadUInt64LE)(&reader, &u64Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u64Val == 0xA1B2C3D4E5F60718ULL);

  CHECK_STATUS(STREAM_FUNC(ReadUInt64BE)(&reader, &u64Val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u64Val == 0xFEDCBA9876543210ULL);

  CHECK_STATUS(STREAM_FUNC(ReadBytes)(&reader, tagRead, 8),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(memcmp(tagRead, "COSMERON", 8) == 0);
  CHECK_TRUE(STREAM_FUNC(IsAtEnd)(&reader));

  return 0;
}

static int Test_BitLevelOperations(void) {
  uint8_t buffer[8] = {0};
  STREAM_TYPE(TStream) stream;

  CHECK_STATUS(STREAM_FUNC(InitWrite)(&stream, buffer, sizeof(buffer)),
               STATUS_CONST(SUCCESS));

  /* Write 3 bits: 5 (binary 101) */
  CHECK_STATUS(STREAM_FUNC(WriteBits)(&stream, 5, 3),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(STREAM_FUNC(Position)(&stream) == 0);
  CHECK_TRUE(STREAM_FUNC(BitPosition)(&stream) == 3);

  /* Write 5 bits: 18 (binary 10010) -> completes first byte: 10110010 (0xB2) */
  CHECK_STATUS(STREAM_FUNC(WriteBits)(&stream, 18, 5),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(STREAM_FUNC(Position)(&stream) == 1);
  CHECK_TRUE(STREAM_FUNC(BitPosition)(&stream) == 0);
  CHECK_TRUE(buffer[0] == 0xB2U);

  /* Write 12 bits: 0xABC (binary 1010 1011 1100) */
  CHECK_STATUS(STREAM_FUNC(WriteBits)(&stream, 0xABCU, 12),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(STREAM_FUNC(Position)(&stream) == 2);
  CHECK_TRUE(STREAM_FUNC(BitPosition)(&stream) == 4);

  /* Align to byte boundary */
  CHECK_STATUS(STREAM_FUNC(AlignToByte)(&stream), STATUS_CONST(SUCCESS));
  CHECK_TRUE(STREAM_FUNC(Position)(&stream) == 3);
  CHECK_TRUE(STREAM_FUNC(BitPosition)(&stream) == 0);

  /* Read-back bit-level verification */
  STREAM_TYPE(TStream) reader;
  CHECK_STATUS(STREAM_FUNC(InitRead)(&reader, buffer, 3),
               STATUS_CONST(SUCCESS));

  uint64_t val = 0;
  CHECK_STATUS(STREAM_FUNC(ReadBits)(&reader, 3, &val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(val == 5U);

  CHECK_STATUS(STREAM_FUNC(ReadBits)(&reader, 5, &val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(val == 18U);

  CHECK_STATUS(STREAM_FUNC(ReadBits)(&reader, 12, &val),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(val == 0xABCU);

  CHECK_STATUS(STREAM_FUNC(AlignToByte)(&reader), STATUS_CONST(SUCCESS));
  CHECK_TRUE(STREAM_FUNC(IsAtEnd)(&reader));

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
