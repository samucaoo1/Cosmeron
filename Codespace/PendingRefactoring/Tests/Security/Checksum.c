#include "../../Cosmeron/Modules/Security/Checksum/Checksum.h"

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

static int TestCRC32(void) {
  /* Standard test vector: "123456789" -> 0xCBF43926 */
  const char *testData = "123456789";
  size_t len = strlen(testData);

  /* One-shot scalar */
  uint32_t crcScalar =
      SECURITY_CHECKSUM_FUNC(CRC32_ComputeUInt32)(0U, testData, len);
  CHECK_TRUE(crcScalar == UINT32_C(0xCBF43926));

  /* One-shot TDigest */
  TDigest(digest);
  CHECK_STATUS(
      SECURITY_CHECKSUM_FUNC(CRC32_Compute)(testData, len, &digest));
  uint32_t val = 0U;
  CHECK_STATUS(SECURITY_DIGEST_FUNC(ToUInt32)(&digest, &val));
  CHECK_TRUE(val == UINT32_C(0xCBF43926));

  /* Streaming incremental */
  SECURITY_CHECKSUM_TYPE(TCRC32) ctx;
  SECURITY_CHECKSUM_FUNC(CRC32_Init)(&ctx);
  SECURITY_CHECKSUM_FUNC(CRC32_Update)(&ctx, testData, 4U);
  SECURITY_CHECKSUM_FUNC(CRC32_Update)(&ctx, testData + 4U, len - 4U);

  TDigest(streamDigest);
  CHECK_STATUS(SECURITY_CHECKSUM_FUNC(CRC32_Final)(&ctx, &streamDigest));
  CHECK_TRUE(SECURITY_DIGEST_FUNC(Equal)(&digest, &streamDigest));

  /* Empty string */
  crcScalar = SECURITY_CHECKSUM_FUNC(CRC32_ComputeUInt32)(0U, "", 0U);
  CHECK_TRUE(crcScalar == 0U);

  return 0;
}

static int TestAdler32(void) {
  /* Standard test vector: "123456789" -> 0x091E01DE */
  const char *testData = "123456789";
  size_t len = strlen(testData);

  /* One-shot scalar */
  uint32_t adlerScalar =
      SECURITY_CHECKSUM_FUNC(Adler32_ComputeUInt32)(1U, testData, len);
  CHECK_TRUE(adlerScalar == UINT32_C(0x091E01DE));

  /* One-shot TDigest */
  TDigest(digest);
  CHECK_STATUS(
      SECURITY_CHECKSUM_FUNC(Adler32_Compute)(testData, len, &digest));
  uint32_t val = 0U;
  CHECK_STATUS(SECURITY_DIGEST_FUNC(ToUInt32)(&digest, &val));
  CHECK_TRUE(val == UINT32_C(0x091E01DE));

  /* Streaming incremental */
  SECURITY_CHECKSUM_TYPE(TAdler32) ctx;
  SECURITY_CHECKSUM_FUNC(Adler32_Init)(&ctx);
  SECURITY_CHECKSUM_FUNC(Adler32_Update)(&ctx, testData, 5U);
  SECURITY_CHECKSUM_FUNC(Adler32_Update)(&ctx, testData + 5U, len - 5U);

  TDigest(streamDigest);
  CHECK_STATUS(SECURITY_CHECKSUM_FUNC(Adler32_Final)(&ctx, &streamDigest));
  CHECK_TRUE(SECURITY_DIGEST_FUNC(Equal)(&digest, &streamDigest));

  /* Empty string */
  adlerScalar = SECURITY_CHECKSUM_FUNC(Adler32_ComputeUInt32)(1U, "", 0U);
  CHECK_TRUE(adlerScalar == 1U);

  return 0;
}

static int TestFNV(void) {
  /* FNV-1 32-bit empty string is offset basis: 2166136261 */
  TDigest(d);
  CHECK_STATUS(SECURITY_CHECKSUM_FUNC(FNV1_32_Compute)("", 0U, &d));
  uint32_t val32 = 0U;
  CHECK_STATUS(SECURITY_DIGEST_FUNC(ToUInt32)(&d, &val32));
  CHECK_TRUE(val32 == SECURITY_CHECKSUM_FNV32_OFFSET_BASIS);

  /* FNV-1a 32-bit for "test" -> 2949673445 (0xAFD071E5) */
  CHECK_STATUS(SECURITY_CHECKSUM_FUNC(FNV1a_32_Compute)("test", 4U, &d));
  CHECK_STATUS(SECURITY_DIGEST_FUNC(ToUInt32)(&d, &val32));
  CHECK_TRUE(val32 == UINT32_C(0xAFD071E5));

  /* Streaming FNV-1a 32-bit */
  SECURITY_CHECKSUM_TYPE(TFNV32) ctx32;
  SECURITY_CHECKSUM_FUNC(FNV1a_32_Init)(&ctx32);
  SECURITY_CHECKSUM_FUNC(FNV1a_32_Update)(&ctx32, "te", 2U);
  SECURITY_CHECKSUM_FUNC(FNV1a_32_Update)(&ctx32, "st", 2U);
  CHECK_TRUE(SECURITY_CHECKSUM_FUNC(FNV1a_32_FinalUInt32)(&ctx32) ==
             UINT32_C(0xAFD071E5));

  /* FNV-1 64-bit empty string */
  TDigest(d64);
  CHECK_STATUS(SECURITY_CHECKSUM_FUNC(FNV1_64_Compute)("", 0U, &d64));
  uint64_t val64 = 0ULL;
  CHECK_STATUS(SECURITY_DIGEST_FUNC(ToUInt64)(&d64, &val64));
  CHECK_TRUE(val64 == SECURITY_CHECKSUM_FNV64_OFFSET_BASIS);

  /* FNV-1a 64-bit streaming vs one-shot */
  SECURITY_CHECKSUM_TYPE(TFNV64) ctx64;
  SECURITY_CHECKSUM_FUNC(FNV1a_64_Init)(&ctx64);
  SECURITY_CHECKSUM_FUNC(FNV1a_64_Update)(&ctx64, "Cosmeron", 8U);
  TDigest(streamD64);
  CHECK_STATUS(SECURITY_CHECKSUM_FUNC(FNV1a_64_Final)(&ctx64, &streamD64));

  TDigest(oneShotD64);
  CHECK_STATUS(
      SECURITY_CHECKSUM_FUNC(FNV1a_64_Compute)("Cosmeron", 8U, &oneShotD64));
  CHECK_TRUE(SECURITY_DIGEST_FUNC(Equal)(&streamD64, &oneShotD64));

  return 0;
}

int main(void) {
  int err = 0;
  if ((err = TestCRC32()) != 0)
    return err;
  if ((err = TestAdler32()) != 0)
    return err;
  if ((err = TestFNV()) != 0)
    return err;

  printf("Security Checksum tests passed.\n");
  return 0;
}
