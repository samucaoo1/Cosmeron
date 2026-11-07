#include "../../Cosmeron/Modules/Security/Digest/Digest.h"

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

static int TestDigestLifecycle(void) {
  TDigest(digest);
  CHECK_TRUE(digest.api != NULL);
  CHECK_TRUE(SECURITY_DIGEST_FUNC(Length)(&digest) == 0U);

  const uint8_t sample[] = {0xDE, 0xAD, 0xBE, 0xEF};
  CHECK_STATUS(SECURITY_DIGEST_FUNC(Init)(&digest, sample, sizeof(sample)));
  CHECK_TRUE(SECURITY_DIGEST_FUNC(Length)(&digest) == 4U);
  CHECK_TRUE(memcmp(SECURITY_DIGEST_FUNC(Bytes)(&digest), sample, 4U) == 0);

  /* Clear */
  SECURITY_DIGEST_FUNC(Clear)(&digest);
  CHECK_TRUE(SECURITY_DIGEST_FUNC(Length)(&digest) == 0U);
  for (size_t i = 0U; i < SECURITY_DIGEST_CONST(MAX_BYTES); ++i)
    CHECK_TRUE(digest.bytes[i] == 0U);

  return 0;
}

static int TestDigestHex(void) {
  TDigest(digest);
  const uint8_t sample[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
  CHECK_STATUS(SECURITY_DIGEST_FUNC(Init)(&digest, sample, sizeof(sample)));

  /* Insufficient space */
  char smallBuf[16];
  CHECK_TRUE(SECURITY_DIGEST_FUNC(ToHex)(&digest, smallBuf, sizeof(smallBuf),
                                         false) ==
             STATUS_CONST(INSUFFICIENT_SPACE));

  /* Lowercase */
  char hexBuf[32];
  CHECK_STATUS(
      SECURITY_DIGEST_FUNC(ToHex)(&digest, hexBuf, sizeof(hexBuf), false));
  CHECK_TRUE(strcmp(hexBuf, "0123456789abcdef") == 0);

  /* Uppercase */
  CHECK_STATUS(
      SECURITY_DIGEST_FUNC(ToHex)(&digest, hexBuf, sizeof(hexBuf), true));
  CHECK_TRUE(strcmp(hexBuf, "0123456789ABCDEF") == 0);

  /* Decode from hex */
  TDigest(decoded);
  CHECK_STATUS(
      SECURITY_DIGEST_FUNC(FromHex)(&decoded, "0123456789abcdef"));
  CHECK_TRUE(SECURITY_DIGEST_FUNC(Equal)(&digest, &decoded));

  CHECK_STATUS(
      SECURITY_DIGEST_FUNC(FromHex)(&decoded, "0123456789ABCDEF"));
  CHECK_TRUE(SECURITY_DIGEST_FUNC(Equal)(&digest, &decoded));

  /* Invalid sequences */
  CHECK_TRUE(SECURITY_DIGEST_FUNC(FromHex)(&decoded, "123") ==
             STATUS_CONST(INVALID_SEQUENCE));
  CHECK_TRUE(SECURITY_DIGEST_FUNC(FromHex)(&decoded, "012g") ==
             STATUS_CONST(INVALID_SEQUENCE));

  return 0;
}

static int TestDigestScalars(void) {
  TDigest(digest);
  const uint8_t sample[] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
  CHECK_STATUS(SECURITY_DIGEST_FUNC(Init)(&digest, sample, sizeof(sample)));

  uint32_t val32 = 0U;
  CHECK_STATUS(SECURITY_DIGEST_FUNC(ToUInt32)(&digest, &val32));
  CHECK_TRUE(val32 == UINT32_C(0x12345678));

  uint64_t val64 = 0ULL;
  CHECK_STATUS(SECURITY_DIGEST_FUNC(ToUInt64)(&digest, &val64));
  CHECK_TRUE(val64 == UINT64_C(0x123456789ABCDEF0));

  /* Short digest fails scalar extraction */
  TDigest(shortDigest);
  CHECK_STATUS(SECURITY_DIGEST_FUNC(Init)(&shortDigest, sample, 2U));
  CHECK_TRUE(SECURITY_DIGEST_FUNC(ToUInt32)(&shortDigest, &val32) ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(SECURITY_DIGEST_FUNC(ToUInt64)(&shortDigest, &val64) ==
             STATUS_CONST(INVALID_ARGUMENT));

  return 0;
}

static int TestDigestEqualCompare(void) {
  TDigest(a);
  TDigest(b);
  const uint8_t dataA[] = {1, 2, 3, 4};
  const uint8_t dataB[] = {1, 2, 3, 5};
  const uint8_t dataShort[] = {1, 2, 3};

  CHECK_STATUS(SECURITY_DIGEST_FUNC(Init)(&a, dataA, sizeof(dataA)));
  CHECK_STATUS(SECURITY_DIGEST_FUNC(Init)(&b, dataA, sizeof(dataA)));
  CHECK_TRUE(SECURITY_DIGEST_FUNC(Equal)(&a, &b));

  CMPOUT cmp = COMPARISON_LOWER_CONST;
  CHECK_STATUS(SECURITY_DIGEST_FUNC(Compare)(&a, &b, &cmp));
  CHECK_TRUE(cmp == COMPARISON_EQUAL_CONST);

  CHECK_STATUS(SECURITY_DIGEST_FUNC(Init)(&b, dataB, sizeof(dataB)));
  CHECK_TRUE(!SECURITY_DIGEST_FUNC(Equal)(&a, &b));
  CHECK_STATUS(SECURITY_DIGEST_FUNC(Compare)(&a, &b, &cmp));
  CHECK_TRUE(cmp == COMPARISON_LOWER_CONST);

  CHECK_STATUS(SECURITY_DIGEST_FUNC(Init)(&b, dataShort, sizeof(dataShort)));
  CHECK_TRUE(!SECURITY_DIGEST_FUNC(Equal)(&a, &b));
  CHECK_STATUS(SECURITY_DIGEST_FUNC(Compare)(&a, &b, &cmp));
  CHECK_TRUE(cmp == COMPARISON_HIGHER_CONST);

  /* FunctionTable dispatch */
  CHECK_STATUS(a.api->init(&a, dataA, sizeof(dataA)));
  CHECK_TRUE(a.api->length(&a) == 4U);
  CHECK_TRUE(a.api->bytes(&a) != NULL);

  return 0;
}

static int TestDigestErrors(void) {
  TDigest(digest);
  uint8_t largeBuf[128];
  memset(largeBuf, 0xAA, sizeof(largeBuf));

  CHECK_TRUE(SECURITY_DIGEST_FUNC(Init)(NULL, largeBuf, 4U) ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(SECURITY_DIGEST_FUNC(Init)(&digest, largeBuf, 65U) ==
             STATUS_CONST(OUT_OF_RANGE));
  CHECK_TRUE(SECURITY_DIGEST_FUNC(ToHex)(NULL, NULL, 0U, false) ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(SECURITY_DIGEST_FUNC(FromHex)(NULL, "00") ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(SECURITY_DIGEST_FUNC(FromHex)(&digest, NULL) ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(SECURITY_DIGEST_FUNC(ToUInt32)(NULL, NULL) ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(SECURITY_DIGEST_FUNC(ToUInt64)(NULL, NULL) ==
             STATUS_CONST(INVALID_ARGUMENT));

  CMPOUT cmp = COMPARISON_EQUAL_CONST;
  CHECK_TRUE(SECURITY_DIGEST_FUNC(Compare)(NULL, &digest, &cmp) ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(SECURITY_DIGEST_FUNC(Compare)(&digest, NULL, &cmp) ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(SECURITY_DIGEST_FUNC(Compare)(&digest, &digest, NULL) ==
             STATUS_CONST(INVALID_ARGUMENT));

  CHECK_TRUE(!SECURITY_DIGEST_FUNC(Equal)(NULL, &digest));
  CHECK_TRUE(!SECURITY_DIGEST_FUNC(Equal)(&digest, NULL));
  CHECK_TRUE(SECURITY_DIGEST_FUNC(Equal)(NULL, NULL));

  return 0;
}

int main(void) {
  int err = 0;
  if ((err = TestDigestLifecycle()) != 0)
    return err;
  if ((err = TestDigestHex()) != 0)
    return err;
  if ((err = TestDigestScalars()) != 0)
    return err;
  if ((err = TestDigestEqualCompare()) != 0)
    return err;
  if ((err = TestDigestErrors()) != 0)
    return err;

  printf("Security Digest tests passed.\n");
  return 0;
}
