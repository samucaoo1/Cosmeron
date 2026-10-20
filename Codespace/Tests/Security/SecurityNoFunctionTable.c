#define SECURITY_DISABLE_FUNCTION_TABLE

#include "../../Cosmeron/Modules/Security/Security.h"

#include <stdio.h>
#include <string.h>

int main(void) {
  TDigest(digest);
  const uint8_t sample[] = {0xAA, 0xBB, 0xCC, 0xDD};

  if (SECURITY_DIGEST_FUNC(Init)(&digest, sample, sizeof(sample)) !=
      STATUS_CONST(SUCCESS))
    return 1;

  char hexBuf[16];
  if (SECURITY_DIGEST_FUNC(ToHex)(&digest, hexBuf, sizeof(hexBuf), false) !=
      STATUS_CONST(SUCCESS))
    return 2;

  if (strcmp(hexBuf, "aabbccdd") != 0)
    return 3;

  TDigest(crcDigest);
  if (SECURITY_CHECKSUM_FUNC(CRC32_Compute)("test", 4U, &crcDigest) !=
      STATUS_CONST(SUCCESS))
    return 4;

  TDigest(md5Digest);
  if (SECURITY_CRYPTO_FUNC(MD5_Compute)("abc", 3U, &md5Digest) !=
      STATUS_CONST(SUCCESS))
    return 5;
  if (md5Digest.length != 16U)
    return 6;

  TDigest(shaDigest);
  if (SECURITY_CRYPTO_FUNC(SHA256_Compute)("abc", 3U, &shaDigest) !=
      STATUS_CONST(SUCCESS))
    return 7;
  if (shaDigest.length != 32U)
    return 8;

  printf("SecurityNoFunctionTable tests passed.\n");
  return 0;
}
