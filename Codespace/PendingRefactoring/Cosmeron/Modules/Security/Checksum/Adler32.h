#pragma once

#include "../Digest/Digest.h"

#define SECURITY_CHECKSUM_ADLER32_BASE UINT32_C(65521)
#define SECURITY_CHECKSUM_ADLER32_NMAX 5552U

typedef struct {
  uint32_t a;
  uint32_t b;
} SECURITY_CHECKSUM_TYPE(TAdler32);

#define SECURITY_CHECKSUM_ADLER32_INIT_PROTOTYPE                               \
  static inline void SECURITY_CHECKSUM_FUNC(Adler32_Init)(                     \
      SECURITY_CHECKSUM_TYPE(TAdler32) * ctx)

#define SECURITY_CHECKSUM_ADLER32_UPDATE_PROTOTYPE                             \
  static inline void SECURITY_CHECKSUM_FUNC(Adler32_Update)(                   \
      SECURITY_CHECKSUM_TYPE(TAdler32) * ctx, const void *data, size_t size)

#define SECURITY_CHECKSUM_ADLER32_FINAL_PROTOTYPE                              \
  static inline OPSTATUS SECURITY_CHECKSUM_FUNC(Adler32_Final)(                \
      SECURITY_CHECKSUM_TYPE(TAdler32) * ctx,                                  \
      SECURITY_TYPE(TDigest) * outDigest)

#define SECURITY_CHECKSUM_ADLER32_FINAL_UINT32_PROTOTYPE                       \
  static inline uint32_t SECURITY_CHECKSUM_FUNC(Adler32_FinalUInt32)(          \
      SECURITY_CHECKSUM_TYPE(TAdler32) * ctx)

#define SECURITY_CHECKSUM_ADLER32_COMPUTE_PROTOTYPE                            \
  static inline OPSTATUS SECURITY_CHECKSUM_FUNC(Adler32_Compute)(              \
      const void *data, size_t size, SECURITY_TYPE(TDigest) *outDigest)

#define SECURITY_CHECKSUM_ADLER32_COMPUTE_UINT32_PROTOTYPE                     \
  static inline uint32_t SECURITY_CHECKSUM_FUNC(Adler32_ComputeUInt32)(        \
      uint32_t previous, const void *data, size_t size)

SECURITY_CHECKSUM_ADLER32_INIT_PROTOTYPE {
  if (ctx) {
    ctx->a = 1U;
    ctx->b = 0U;
  }
}

SECURITY_CHECKSUM_ADLER32_UPDATE_PROTOTYPE {
  if (!ctx || !data || size == 0U)
    return;

  const uint8_t *bytes = (const uint8_t *)data;
  uint32_t a = ctx->a;
  uint32_t b = ctx->b;

  while (size > 0U) {
    size_t count = size > SECURITY_CHECKSUM_ADLER32_NMAX
                       ? SECURITY_CHECKSUM_ADLER32_NMAX
                       : size;
    for (size_t i = 0U; i < count; ++i) {
      a += bytes[i];
      b += a;
    }
    a %= SECURITY_CHECKSUM_ADLER32_BASE;
    b %= SECURITY_CHECKSUM_ADLER32_BASE;
    bytes += count;
    size -= count;
  }

  ctx->a = a;
  ctx->b = b;
}

SECURITY_CHECKSUM_ADLER32_FINAL_UINT32_PROTOTYPE {
  return ctx ? ((ctx->b << 16U) | ctx->a) : 0U;
}

SECURITY_CHECKSUM_ADLER32_FINAL_PROTOTYPE {
  if (!ctx || !outDigest)
    return STATUS_CONST(INVALID_ARGUMENT);

  uint32_t result = (ctx->b << 16U) | ctx->a;
  uint8_t raw[4];
  raw[0] = (uint8_t)((result >> 24U) & 0xFFU);
  raw[1] = (uint8_t)((result >> 16U) & 0xFFU);
  raw[2] = (uint8_t)((result >> 8U) & 0xFFU);
  raw[3] = (uint8_t)(result & 0xFFU);

  return SECURITY_DIGEST_FUNC(Init)(outDigest, raw, sizeof(raw));
}

SECURITY_CHECKSUM_ADLER32_COMPUTE_PROTOTYPE {
  if (!outDigest)
    return STATUS_CONST(INVALID_ARGUMENT);

  SECURITY_CHECKSUM_TYPE(TAdler32) ctx;
  SECURITY_CHECKSUM_FUNC(Adler32_Init)(&ctx);
  SECURITY_CHECKSUM_FUNC(Adler32_Update)(&ctx, data, size);
  return SECURITY_CHECKSUM_FUNC(Adler32_Final)(&ctx, outDigest);
}

SECURITY_CHECKSUM_ADLER32_COMPUTE_UINT32_PROTOTYPE {
  const uint8_t *bytes = (const uint8_t *)data;
  if (!bytes || size == 0U)
    return previous;

  uint32_t a = previous & UINT32_C(0xFFFF);
  uint32_t b = (previous >> 16U) & UINT32_C(0xFFFF);

  while (size > 0U) {
    size_t count = size > SECURITY_CHECKSUM_ADLER32_NMAX
                       ? SECURITY_CHECKSUM_ADLER32_NMAX
                       : size;
    for (size_t i = 0U; i < count; ++i) {
      a += bytes[i];
      b += a;
    }
    a %= SECURITY_CHECKSUM_ADLER32_BASE;
    b %= SECURITY_CHECKSUM_ADLER32_BASE;
    bytes += count;
    size -= count;
  }

  return (b << 16U) | a;
}
/* EOF */
