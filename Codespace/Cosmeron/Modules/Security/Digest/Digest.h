#pragma once
#include <stdint.h>

#include "../Security.space"

#ifndef SECURITY_DISABLE_FUNCTION_TABLE
#define SECURITY_DIGEST_FUNCTION_TABLE_FORWARD                                 \
  struct SECURITY_DIGEST_TYPE(FunctionTable);
#define SECURITY_DIGEST_API_MEMBER                                             \
  const struct SECURITY_DIGEST_TYPE(FunctionTable) * api;
#else
#define SECURITY_DIGEST_FUNCTION_TABLE_FORWARD
#define SECURITY_DIGEST_API_MEMBER
#endif

SECURITY_DIGEST_FUNCTION_TABLE_FORWARD
typedef struct SECURITY_DIGEST_TYPE(Digest) {
  SECURITY_DIGEST_API_MEMBER
  uint8_t bytes[SECURITY_DIGEST_CONST(MAX_BYTES)];
  size_t length;
} SECURITY_DIGEST_TYPE(TDigest);

typedef SECURITY_DIGEST_TYPE(TDigest) SECURITY_TYPE(TDigest);

#define SECURITY_DIGEST_INIT_PROTOTYPE                                         \
  static inline OPSTATUS SECURITY_DIGEST_FUNC(Init)(                           \
      SECURITY_DIGEST_TYPE(TDigest) * digest, const void *bytes,               \
      size_t length)

#define SECURITY_DIGEST_CLEAR_PROTOTYPE                                        \
  static inline void SECURITY_DIGEST_FUNC(Clear)(                              \
      SECURITY_DIGEST_TYPE(TDigest) * digest)

#define SECURITY_DIGEST_TO_HEX_PROTOTYPE                                       \
  static inline OPSTATUS SECURITY_DIGEST_FUNC(ToHex)(                          \
      const SECURITY_DIGEST_TYPE(TDigest) * digest, char *dest,                \
      size_t capacity, bool uppercase)

#define SECURITY_DIGEST_FROM_HEX_PROTOTYPE                                     \
  static inline OPSTATUS SECURITY_DIGEST_FUNC(FromHex)(                        \
      SECURITY_DIGEST_TYPE(TDigest) * digest, const char *hex)

#define SECURITY_DIGEST_TO_UINT32_PROTOTYPE                                    \
  static inline OPSTATUS SECURITY_DIGEST_FUNC(ToUInt32)(                       \
      const SECURITY_DIGEST_TYPE(TDigest) * digest, uint32_t *outValue)

#define SECURITY_DIGEST_TO_UINT64_PROTOTYPE                                    \
  static inline OPSTATUS SECURITY_DIGEST_FUNC(ToUInt64)(                       \
      const SECURITY_DIGEST_TYPE(TDigest) * digest, uint64_t *outValue)

#define SECURITY_DIGEST_BYTES_PROTOTYPE                                        \
  static inline const uint8_t *SECURITY_DIGEST_FUNC(Bytes)(                    \
      const SECURITY_DIGEST_TYPE(TDigest) * digest)

#define SECURITY_DIGEST_LENGTH_PROTOTYPE                                       \
  static inline size_t SECURITY_DIGEST_FUNC(Length)(                           \
      const SECURITY_DIGEST_TYPE(TDigest) * digest)

#define SECURITY_DIGEST_EQUAL_PROTOTYPE                                        \
  static inline bool SECURITY_DIGEST_FUNC(Equal)(                              \
      const SECURITY_DIGEST_TYPE(TDigest) * digestA,                           \
      const SECURITY_DIGEST_TYPE(TDigest) * digestB)

#define SECURITY_DIGEST_COMPARE_PROTOTYPE                                      \
  static inline OPSTATUS SECURITY_DIGEST_FUNC(Compare)(                        \
      const SECURITY_DIGEST_TYPE(TDigest) * digestA,                           \
      const SECURITY_DIGEST_TYPE(TDigest) * digestB, CMPOUT * outResult)

SECURITY_DIGEST_INIT_PROTOTYPE;
SECURITY_DIGEST_CLEAR_PROTOTYPE;
SECURITY_DIGEST_TO_HEX_PROTOTYPE;
SECURITY_DIGEST_FROM_HEX_PROTOTYPE;
SECURITY_DIGEST_TO_UINT32_PROTOTYPE;
SECURITY_DIGEST_TO_UINT64_PROTOTYPE;
SECURITY_DIGEST_BYTES_PROTOTYPE;
SECURITY_DIGEST_LENGTH_PROTOTYPE;
SECURITY_DIGEST_EQUAL_PROTOTYPE;
SECURITY_DIGEST_COMPARE_PROTOTYPE;

#include "Impl/Digest.impl"

#ifndef SECURITY_DISABLE_FUNCTION_TABLE
#include "FunctionTable/Digest.h"
SECURITY_DIGEST_FUNCTION_TABLE_INSTANCE
#endif

#ifndef SECURITY_DISABLE_FUNCTION_TABLE
#define TDigest(NAME)                                                          \
  SECURITY_DIGEST_TYPE(TDigest) NAME = {0};                                    \
  NAME.api = &SECURITY_DIGEST_FUNC(functions);
#else
#define TDigest(NAME) SECURITY_DIGEST_TYPE(TDigest) NAME = {0};
#endif
/* EOF */
