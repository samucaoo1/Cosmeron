#include "../../Cosmeron/Modules/Security/Crypto/Crypto.h"

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

static int VerifyDigestHex(const SECURITY_TYPE(TDigest) * digest,
                           const char *expectedHex) {
  char hex[SECURITY_DIGEST_CONST(MAX_HEX_LENGTH)];
  CHECK_STATUS(
      SECURITY_DIGEST_FUNC(ToHex)(digest, hex, sizeof(hex), false));
  CHECK_TRUE(strcmp(hex, expectedHex) == 0);
  return 0;
}

static int TestMD5Vectors(void) {
  TDigest(d);

  /* RFC 1321 Vector 1: "" */
  CHECK_STATUS(SECURITY_CRYPTO_FUNC(MD5_Compute)("", 0U, &d));
  CHECK_TRUE(d.length == 16U);
  if (VerifyDigestHex(&d, "d41d8cd98f00b204e9800998ecf8427e") != 0)
    return __LINE__;

  /* RFC 1321 Vector 2: "a" */
  CHECK_STATUS(SECURITY_CRYPTO_FUNC(MD5_Compute)("a", 1U, &d));
  if (VerifyDigestHex(&d, "0cc175b9c0f1b6a831c399e269772661") != 0)
    return __LINE__;

  /* RFC 1321 Vector 3: "abc" */
  CHECK_STATUS(SECURITY_CRYPTO_FUNC(MD5_Compute)("abc", 3U, &d));
  if (VerifyDigestHex(&d, "900150983cd24fb0d6963f7d28e17f72") != 0)
    return __LINE__;

  /* RFC 1321 Vector 4: "message digest" */
  const char *msg = "message digest";
  CHECK_STATUS(SECURITY_CRYPTO_FUNC(MD5_Compute)(msg, strlen(msg), &d));
  if (VerifyDigestHex(&d, "f96b697d7cb7938d525a2f31aaf161d0") != 0)
    return __LINE__;

  /* RFC 1321 Vector 5: "abcdefghijklmnopqrstuvwxyz" */
  const char *alpha = "abcdefghijklmnopqrstuvwxyz";
  CHECK_STATUS(SECURITY_CRYPTO_FUNC(MD5_Compute)(alpha, strlen(alpha), &d));
  if (VerifyDigestHex(&d, "c3fcd3d76192e4007dfb496cca67e13b") != 0)
    return __LINE__;

  /* Streaming incremental */
  SECURITY_CRYPTO_TYPE(TMD5) ctx;
  SECURITY_CRYPTO_FUNC(MD5_Init)(&ctx);
  SECURITY_CRYPTO_FUNC(MD5_Update)(&ctx, "message ", 8U);
  SECURITY_CRYPTO_FUNC(MD5_Update)(&ctx, "digest", 6U);
  TDigest(streamD);
  CHECK_STATUS(SECURITY_CRYPTO_FUNC(MD5_Final)(&ctx, &streamD));
  CHECK_TRUE(SECURITY_DIGEST_FUNC(Equal)(&d, &streamD) == false); /* diff from alpha */

  CHECK_STATUS(SECURITY_CRYPTO_FUNC(MD5_Compute)(msg, strlen(msg), &d));
  CHECK_TRUE(SECURITY_DIGEST_FUNC(Equal)(&d, &streamD));

  return 0;
}

static int TestSHA256Vectors(void) {
  TDigest(d);

  /* NIST FIPS 180-4 Vector 1: "" */
  CHECK_STATUS(SECURITY_CRYPTO_FUNC(SHA256_Compute)("", 0U, &d));
  CHECK_TRUE(d.length == 32U);
  if (VerifyDigestHex(
          &d, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855") !=
      0)
    return __LINE__;

  /* NIST FIPS 180-4 Vector 2: "abc" */
  CHECK_STATUS(SECURITY_CRYPTO_FUNC(SHA256_Compute)("abc", 3U, &d));
  if (VerifyDigestHex(
          &d, "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad") !=
      0)
    return __LINE__;

  /* NIST FIPS 180-4 Vector 3: 56-byte message */
  const char *msg56 =
      "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
  CHECK_STATUS(
      SECURITY_CRYPTO_FUNC(SHA256_Compute)(msg56, strlen(msg56), &d));
  if (VerifyDigestHex(
          &d, "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1") !=
      0)
    return __LINE__;

  /* Streaming incremental */
  SECURITY_CRYPTO_TYPE(TSHA256) ctx;
  SECURITY_CRYPTO_FUNC(SHA256_Init)(&ctx);
  SECURITY_CRYPTO_FUNC(SHA256_Update)(&ctx, msg56, 20U);
  SECURITY_CRYPTO_FUNC(SHA256_Update)(&ctx, msg56 + 20U, strlen(msg56) - 20U);
  TDigest(streamD);
  CHECK_STATUS(SECURITY_CRYPTO_FUNC(SHA256_Final)(&ctx, &streamD));
  CHECK_TRUE(SECURITY_DIGEST_FUNC(Equal)(&d, &streamD));

  return 0;
}

static int TestCryptoErrors(void) {
  TDigest(d);
  CHECK_TRUE(SECURITY_CRYPTO_FUNC(MD5_Compute)(NULL, 0U, NULL) ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(SECURITY_CRYPTO_FUNC(MD5_Final)(NULL, &d) ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(SECURITY_CRYPTO_FUNC(SHA256_Compute)(NULL, 0U, NULL) ==
             STATUS_CONST(INVALID_ARGUMENT));
  CHECK_TRUE(SECURITY_CRYPTO_FUNC(SHA256_Final)(NULL, &d) ==
             STATUS_CONST(INVALID_ARGUMENT));
  return 0;
}

int main(void) {
  int err = 0;
  if ((err = TestMD5Vectors()) != 0)
    return err;
  if ((err = TestSHA256Vectors()) != 0)
    return err;
  if ((err = TestCryptoErrors()) != 0)
    return err;

  printf("Security Crypto tests passed.\n");
  return 0;
}
