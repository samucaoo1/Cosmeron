#include "../../Cosmeron/Modules/Type/Tag.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define CHECK_STATUS(EXPR)                                                     \
  do {                                                                         \
    if ((EXPR) != STATUS_CONST(SUCCESS)) {                                     \
      fprintf(stderr, "Check failed at %s:%d\n", __FILE__, __LINE__);         \
      return __LINE__;                                                         \
    }                                                                          \
  } while (0)

#define CHECK_TRUE(EXPR)                                                       \
  do {                                                                         \
    if (!(EXPR)) {                                                             \
      fprintf(stderr, "Check failed at %s:%d\n", __FILE__, __LINE__);         \
      return __LINE__;                                                         \
    }                                                                          \
  } while (0)

static int TestTagLiterals(void) {
  uint16_t tag16 = TAG16_MAKE('M', 'Z');
  CHECK_TRUE(tag16 == 0x4D5AU);

  uint32_t tag32 = TAG32_MAKE('R', 'I', 'F', 'F');
  CHECK_TRUE(tag32 == 0x52494646U);

  uint32_t tagAlias = TAG_MAKE('R', 'I', 'F', 'F');
  CHECK_TRUE(tagAlias == tag32);

  uint64_t tag64 = TAG64_MAKE('S', 'Q', 'L', 'i', 't', 'e', ' ', '3');
  CHECK_TRUE(tag64 == UINT64_C(0x53514C6974652033));

  /* Test compile-time switch-case usage */
  int matched = 0;
  switch (tag32) {
  case TAG32_MAKE('W', 'A', 'V', 'E'):
    matched = 1;
    break;
  case TAG32_MAKE('R', 'I', 'F', 'F'):
    matched = 2;
    break;
  default:
    matched = -1;
    break;
  }
  CHECK_TRUE(matched == 2);

  return 0;
}

static int TestTag16(void) {
  TTag16(tag);
  CHECK_TRUE(tag.api != NULL);

  CHECK_STATUS(TAG_FUNC(16, Init)(&tag, "MZ"));
  CHECK_TRUE(tag.value == TAG16_MAKE('M', 'Z'));
  CHECK_TRUE(strcmp(tag.text, "MZ") == 0);
  CHECK_TRUE(TAG_FUNC(16, ToUInt)(&tag) == TAG16_MAKE('M', 'Z'));
  CHECK_TRUE(strcmp(TAG_FUNC(16, Text)(&tag), "MZ") == 0);
  CHECK_TRUE(TAG_FUNC(16, IsPrintable)(&tag));

  /* Padding when string is shorter than 2 bytes */
  CHECK_STATUS(TAG_FUNC(16, Init)(&tag, "M"));
  CHECK_TRUE(tag.value == TAG16_MAKE('M', ' '));
  CHECK_TRUE(strcmp(tag.text, "M ") == 0);

  /* FromUInt */
  CHECK_STATUS(TAG_FUNC(16, FromUInt)(&tag, 0x424DU)); /* "BM" */
  CHECK_TRUE(strcmp(tag.text, "BM") == 0);
  CHECK_TRUE(tag.value == 0x424DU);

  /* FromBytes */
  const uint8_t pkBytes[] = {'P', 'K'};
  CHECK_STATUS(TAG_FUNC(16, FromBytes)(&tag, pkBytes, sizeof(pkBytes)));
  CHECK_TRUE(tag.value == TAG16_MAKE('P', 'K'));

  /* ReadBE & ReadLE */
  const uint8_t rawBE[] = {0x12, 0x34};
  CHECK_STATUS(TAG_FUNC(16, ReadBE)(&tag, rawBE));
  CHECK_TRUE(tag.value == 0x1234U);

  const uint8_t rawLE[] = {0x34, 0x12};
  CHECK_STATUS(TAG_FUNC(16, ReadLE)(&tag, rawLE));
  CHECK_TRUE(tag.value == 0x1234U);

  /* WriteBE & WriteLE */
  uint8_t outBE[2] = {0};
  CHECK_STATUS(TAG_FUNC(16, WriteBE)(&tag, outBE));
  CHECK_TRUE(outBE[0] == 0x12 && outBE[1] == 0x34);

  uint8_t outLE[2] = {0};
  CHECK_STATUS(TAG_FUNC(16, WriteLE)(&tag, outLE));
  CHECK_TRUE(outLE[0] == 0x34 && outLE[1] == 0x12);

  /* ToString */
  char bufSmall[2];
  CHECK_TRUE(TAG_FUNC(16, ToString)(&tag, bufSmall, sizeof(bufSmall)) ==
             STATUS_CONST(INSUFFICIENT_SPACE));

  char strBuf[8];
  CHECK_STATUS(TAG_FUNC(16, ToString)(&tag, strBuf, sizeof(strBuf)));
  /* Non-printable byte 0x12 replaced by '.' */
  CHECK_TRUE(strBuf[0] == '.' && strBuf[1] == '4' && strBuf[2] == '\0');

  /* Equal & Compare */
  TTag16(other);
  CHECK_STATUS(TAG_FUNC(16, FromUInt)(&other, 0x1234U));
  CHECK_TRUE(TAG_FUNC(16, Equal)(&tag, &other));

  CMPOUT cmp = COMPARISON_EQUAL_CONST;
  CHECK_STATUS(TAG_FUNC(16, Compare)(&tag, &other, &cmp));
  CHECK_TRUE(cmp == COMPARISON_EQUAL_CONST);

  CHECK_STATUS(TAG_FUNC(16, FromUInt)(&other, 0x1235U));
  CHECK_TRUE(!TAG_FUNC(16, Equal)(&tag, &other));
  CHECK_STATUS(TAG_FUNC(16, Compare)(&tag, &other, &cmp));
  CHECK_TRUE(cmp == COMPARISON_LOWER_CONST);

  /* FunctionTable dispatch */
  CHECK_STATUS(tag.api->init(&tag, "MZ"));
  CHECK_TRUE(tag.value == TAG16_MAKE('M', 'Z'));
  CHECK_TRUE(tag.api->isPrintable(&tag));
  CHECK_TRUE(tag.api->toUInt(&tag) == TAG16_MAKE('M', 'Z'));

  return 0;
}

static int TestTag32(void) {
  TTag32(tag);
  CHECK_TRUE(tag.api != NULL);

  CHECK_STATUS(TAG_FUNC(32, Init)(&tag, "RIFF"));
  CHECK_TRUE(tag.value == TAG32_MAKE('R', 'I', 'F', 'F'));
  CHECK_TRUE(strcmp(tag.text, "RIFF") == 0);
  CHECK_TRUE(TAG_FUNC(32, IsPrintable)(&tag));
  CHECK_TRUE(strcmp(TAG_FUNC(32, Text)(&tag), "RIFF") == 0);

  /* Short input space padded */
  CHECK_STATUS(TAG_FUNC(32, Init)(&tag, "PNG"));
  CHECK_TRUE(tag.value == TAG32_MAKE('P', 'N', 'G', ' '));
  CHECK_TRUE(strcmp(tag.text, "PNG ") == 0);

  /* FromUInt */
  CHECK_STATUS(TAG_FUNC(32, FromUInt)(&tag, TAG32_MAKE('W', 'A', 'V', 'E')));
  CHECK_TRUE(strcmp(tag.text, "WAVE") == 0);

  /* ReadBE & ReadLE */
  const uint8_t raw[4] = {0x11, 0x22, 0x33, 0x44};
  CHECK_STATUS(TAG_FUNC(32, ReadBE)(&tag, raw));
  CHECK_TRUE(tag.value == 0x11223344U);

  CHECK_STATUS(TAG_FUNC(32, ReadLE)(&tag, raw));
  CHECK_TRUE(tag.value == 0x44332211U);

  /* WriteBE & WriteLE */
  uint8_t outBE[4] = {0};
  CHECK_STATUS(TAG_FUNC(32, WriteBE)(&tag, outBE));
  CHECK_TRUE(outBE[0] == 0x44 && outBE[1] == 0x33 && outBE[2] == 0x22 &&
             outBE[3] == 0x11);

  uint8_t outLE[4] = {0};
  CHECK_STATUS(TAG_FUNC(32, WriteLE)(&tag, outLE));
  CHECK_TRUE(outLE[0] == 0x11 && outLE[1] == 0x22 && outLE[2] == 0x33 &&
             outLE[3] == 0x44);

  /* ToString */
  char bufSmall[4];
  CHECK_TRUE(TAG_FUNC(32, ToString)(&tag, bufSmall, sizeof(bufSmall)) ==
             STATUS_CONST(INSUFFICIENT_SPACE));

  char strBuf[16];
  CHECK_STATUS(TAG_FUNC(32, Init)(&tag, "JPEG"));
  CHECK_STATUS(TAG_FUNC(32, ToString)(&tag, strBuf, sizeof(strBuf)));
  CHECK_TRUE(strcmp(strBuf, "JPEG") == 0);

  /* Equal and Compare */
  TTag32(riff);
  TTag32(wave);
  CHECK_STATUS(TAG_FUNC(32, Init)(&riff, "RIFF"));
  CHECK_STATUS(TAG_FUNC(32, Init)(&wave, "WAVE"));

  CHECK_TRUE(!TAG_FUNC(32, Equal)(&riff, &wave));
  CHECK_TRUE(TAG_FUNC(32, Equal)(&riff, &riff));

  CMPOUT cmp = COMPARISON_EQUAL_CONST;
  CHECK_STATUS(TAG_FUNC(32, Compare)(&riff, &wave, &cmp));
  CHECK_TRUE(cmp == COMPARISON_LOWER_CONST);

  CHECK_STATUS(TAG_FUNC(32, Compare)(&wave, &riff, &cmp));
  CHECK_TRUE(cmp == COMPARISON_HIGHER_CONST);

  CHECK_STATUS(TAG_FUNC(32, Compare)(&riff, &riff, &cmp));
  CHECK_TRUE(cmp == COMPARISON_EQUAL_CONST);

  /* Canonical TTag alias */
  TYPE_NS(TTag) canonicalTag = {0};
  canonicalTag.api = &TAG_FUNC(32, functions);
  CHECK_STATUS(canonicalTag.api->init(&canonicalTag, "OGG "));
  CHECK_TRUE(canonicalTag.value == TAG32_MAKE('O', 'G', 'G', ' '));

  return 0;
}

static int TestTag64(void) {
  TTag64(tag);
  CHECK_TRUE(tag.api != NULL);

  CHECK_STATUS(TAG_FUNC(64, Init)(&tag, "SQLite 3"));
  CHECK_TRUE(tag.value == TAG64_MAKE('S', 'Q', 'L', 'i', 't', 'e', ' ', '3'));
  CHECK_TRUE(strcmp(tag.text, "SQLite 3") == 0);
  CHECK_TRUE(TAG_FUNC(64, IsPrintable)(&tag));

  /* Short input */
  CHECK_STATUS(TAG_FUNC(64, Init)(&tag, "Cosm"));
  CHECK_TRUE(strcmp(tag.text, "Cosm    ") == 0);

  /* ReadBE & ReadLE */
  const uint8_t raw[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  CHECK_STATUS(TAG_FUNC(64, ReadBE)(&tag, raw));
  CHECK_TRUE(tag.value == UINT64_C(0x0102030405060708));

  CHECK_STATUS(TAG_FUNC(64, ReadLE)(&tag, raw));
  CHECK_TRUE(tag.value == UINT64_C(0x0807060504030201));

  /* WriteBE & WriteLE */
  uint8_t outBE[8] = {0};
  CHECK_STATUS(TAG_FUNC(64, WriteBE)(&tag, outBE));
  CHECK_TRUE(outBE[0] == 8 && outBE[7] == 1);

  uint8_t outLE[8] = {0};
  CHECK_STATUS(TAG_FUNC(64, WriteLE)(&tag, outLE));
  CHECK_TRUE(outLE[0] == 1 && outLE[7] == 8);

  /* FunctionTable dispatch */
  CHECK_STATUS(tag.api->init(&tag, "SQLITE3!"));
  CHECK_TRUE(tag.api->isPrintable(&tag));
  CHECK_TRUE(strcmp(tag.api->text(&tag), "SQLITE3!") == 0);

  return 0;
}

static int TestErrorHandling(void) {
  TTag32(tag);
  CMPOUT cmp = COMPARISON_EQUAL_CONST;

  CHECK_TRUE(TAG_FUNC(32, Init)(NULL, "RIFF") ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(TAG_FUNC(32, Init)(&tag, NULL) == STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(TAG_FUNC(32, FromUInt)(NULL, 0U) ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(TAG_FUNC(32, FromBytes)(NULL, "RIFF", 4U) ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(TAG_FUNC(32, FromBytes)(&tag, NULL, 4U) ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(TAG_FUNC(32, ReadBE)(NULL, "RIFF") ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(TAG_FUNC(32, ReadBE)(&tag, NULL) ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(TAG_FUNC(32, ReadLE)(NULL, "RIFF") ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(TAG_FUNC(32, ReadLE)(&tag, NULL) ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(TAG_FUNC(32, WriteBE)(NULL, &tag) ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(TAG_FUNC(32, WriteBE)(&tag, NULL) ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(TAG_FUNC(32, WriteLE)(NULL, &tag) ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(TAG_FUNC(32, WriteLE)(&tag, NULL) ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(TAG_FUNC(32, ToString)(NULL, NULL, 0U) ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(TAG_FUNC(32, Compare)(NULL, &tag, &cmp) ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(TAG_FUNC(32, Compare)(&tag, NULL, &cmp) ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(TAG_FUNC(32, Compare)(&tag, &tag, NULL) ==
             STATUS_CONST(INVALID_ARGUMENT));

  /* Safe null behavior on queries */
  CHECK_TRUE(TAG_FUNC(32, ToUInt)(NULL) == 0U);
  CHECK_TRUE(strcmp(TAG_FUNC(32, Text)(NULL), "") == 0);
  CHECK_TRUE(!TAG_FUNC(32, IsPrintable)(NULL));
  CHECK_TRUE(!TAG_FUNC(32, Equal)(NULL, &tag));
  CHECK_TRUE(!TAG_FUNC(32, Equal)(&tag, NULL));
  CHECK_TRUE(TAG_FUNC(32, Equal)(NULL, NULL));

  return 0;
}

int main(void) {
  int err = 0;
  if ((err = TestTagLiterals()) != 0)
    return err;
  if ((err = TestTag16()) != 0)
    return err;
  if ((err = TestTag32()) != 0)
    return err;
  if ((err = TestTag64()) != 0)
    return err;
  if ((err = TestErrorHandling()) != 0)
    return err;

  printf("Tag tests passed successfully.\n");
  return 0;
}
