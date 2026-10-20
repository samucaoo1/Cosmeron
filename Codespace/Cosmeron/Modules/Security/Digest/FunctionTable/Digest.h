#pragma once

#ifndef SECURITY_DISABLE_FUNCTION_TABLE
#include <stdint.h>
typedef struct SECURITY_DIGEST_TYPE(FunctionTable) {
  OPSTATUS (*init)(SECURITY_DIGEST_TYPE(TDigest) * digest, const void *bytes,
                   size_t length);
  void (*clear)(SECURITY_DIGEST_TYPE(TDigest) * digest);
  OPSTATUS (*toHex)(const SECURITY_DIGEST_TYPE(TDigest) * digest, char *dest,
                    size_t capacity, bool uppercase);
  OPSTATUS (*fromHex)(SECURITY_DIGEST_TYPE(TDigest) * digest, const char *hex);
  OPSTATUS (*toUInt32)(const SECURITY_DIGEST_TYPE(TDigest) * digest,
                       uint32_t *outValue);
  OPSTATUS (*toUInt64)(const SECURITY_DIGEST_TYPE(TDigest) * digest,
                       uint64_t *outValue);
  const uint8_t *(*bytes)(const SECURITY_DIGEST_TYPE(TDigest) * digest);
  size_t (*length)(const SECURITY_DIGEST_TYPE(TDigest) * digest);
  bool (*equal)(const SECURITY_DIGEST_TYPE(TDigest) * digestA,
                const SECURITY_DIGEST_TYPE(TDigest) * digestB);
  OPSTATUS (*compare)(const SECURITY_DIGEST_TYPE(TDigest) * digestA,
                      const SECURITY_DIGEST_TYPE(TDigest) * digestB,
                      CMPOUT *outResult);
} SECURITY_DIGEST_TYPE(FunctionTable);

#define SECURITY_DIGEST_FUNCTION_TABLE_INSTANCE                                \
  static const SECURITY_DIGEST_TYPE(FunctionTable) SECURITY_DIGEST_FUNC(       \
      functions) = {.init = SECURITY_DIGEST_FUNC(Init),                        \
                    .clear = SECURITY_DIGEST_FUNC(Clear),                      \
                    .toHex = SECURITY_DIGEST_FUNC(ToHex),                      \
                    .fromHex = SECURITY_DIGEST_FUNC(FromHex),                  \
                    .toUInt32 = SECURITY_DIGEST_FUNC(ToUInt32),                \
                    .toUInt64 = SECURITY_DIGEST_FUNC(ToUInt64),                \
                    .bytes = SECURITY_DIGEST_FUNC(Bytes),                      \
                    .length = SECURITY_DIGEST_FUNC(Length),                    \
                    .equal = SECURITY_DIGEST_FUNC(Equal),                      \
                    .compare = SECURITY_DIGEST_FUNC(Compare)};

#endif
/* EOF */
