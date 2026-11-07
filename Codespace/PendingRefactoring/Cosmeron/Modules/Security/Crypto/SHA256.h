#pragma once

#include "../Digest/Digest.h"

enum {
  SECURITY_CRYPTO_CONST(SHA256_DIGEST_BYTES) = 32U,
  SECURITY_CRYPTO_CONST(SHA256_BLOCK_BYTES) = 64U
};

typedef struct {
  uint32_t state[8];
  uint64_t bytes;
  uint8_t buffer[SECURITY_CRYPTO_CONST(SHA256_BLOCK_BYTES)];
  size_t filled;
} SECURITY_CRYPTO_TYPE(TSHA256);

#define SECURITY_CRYPTO_SHA256_INIT_PROTOTYPE                                  \
  static inline void SECURITY_CRYPTO_FUNC(SHA256_Init)(                        \
      SECURITY_CRYPTO_TYPE(TSHA256) * ctx)

#define SECURITY_CRYPTO_SHA256_UPDATE_PROTOTYPE                                \
  static inline void SECURITY_CRYPTO_FUNC(SHA256_Update)(                      \
      SECURITY_CRYPTO_TYPE(TSHA256) * ctx, const void *data, size_t size)

#define SECURITY_CRYPTO_SHA256_FINAL_PROTOTYPE                                 \
  static inline OPSTATUS SECURITY_CRYPTO_FUNC(SHA256_Final)(                   \
      SECURITY_CRYPTO_TYPE(TSHA256) * ctx, SECURITY_TYPE(TDigest) * outDigest)

#define SECURITY_CRYPTO_SHA256_COMPUTE_PROTOTYPE                               \
  static inline OPSTATUS SECURITY_CRYPTO_FUNC(SHA256_Compute)(                 \
      const void *data, size_t size, SECURITY_TYPE(TDigest) *outDigest)

static inline uint32_t SECURITY_INS(SHA256_RotR)(uint32_t x, unsigned int n) {
  return (x >> n) | (x << (32U - n));
}

static inline void SECURITY_INS(SHA256_ProcessBlock)(
    SECURITY_CRYPTO_TYPE(TSHA256) * ctx) {
  static const uint32_t k[64] = {
      0x428A2F98U, 0x71374491U, 0xB5C0FBCFU, 0xE9B5DBA5U, 0x3956C25BU,
      0x59F111F1U, 0x923F82A4U, 0xAB1C5ED5U, 0xD807AA98U, 0x12835B01U,
      0x243185BEU, 0x550C7DC3U, 0x72BE5D74U, 0x80DEB1FEU, 0x9BDC06A7U,
      0xC19BF174U, 0xE49B69C1U, 0xEFBE4786U, 0x0FC19DC6U, 0x240CA1CCU,
      0x2DE92C6FU, 0x4A7484AAU, 0x5CB0A9DCU, 0x76F988DAU, 0x983E5152U,
      0xA831C66DU, 0xB00327C8U, 0xBF597FC7U, 0xC6E00BF3U, 0xD5A79147U,
      0x06CA6351U, 0x14292967U, 0x27B70A85U, 0x2E1B2138U, 0x4D2C6DFCU,
      0x53380D13U, 0x650A7354U, 0x766A0ABBU, 0x81C2C92EU, 0x92722C85U,
      0xA2BFE8A1U, 0xA81A664BU, 0xC24B8B70U, 0xC76C51A3U, 0xD192E819U,
      0xD6990624U, 0xF40E3585U, 0x106AA070U, 0x19A4C116U, 0x1E376C08U,
      0x2748774CU, 0x34B0BCB5U, 0x391C0CB3U, 0x4ED8AA4AU, 0x5B9CCA4FU,
      0x682E6FF3U, 0x748F82EEU, 0x78A5636FU, 0x84C87814U, 0x8CC70208U,
      0x90BEFFFAU, 0xA4506CEBU, 0xBEF9A3F7U, 0xC67178F2U};

  uint32_t w[64];
  for (size_t i = 0U; i < 16U; ++i) {
    const uint8_t *p = ctx->buffer + (4U * i);
    w[i] = ((uint32_t)p[0] << 24U) | ((uint32_t)p[1] << 16U) |
           ((uint32_t)p[2] << 8U) | (uint32_t)p[3];
  }

  for (size_t i = 16U; i < 64U; ++i) {
    uint32_t s0 = SECURITY_INS(SHA256_RotR)(w[i - 15U], 7U) ^
                  SECURITY_INS(SHA256_RotR)(w[i - 15U], 18U) ^
                  (w[i - 15U] >> 3U);
    uint32_t s1 = SECURITY_INS(SHA256_RotR)(w[i - 2U], 17U) ^
                  SECURITY_INS(SHA256_RotR)(w[i - 2U], 19U) ^ (w[i - 2U] >> 10U);
    w[i] = w[i - 16U] + s0 + w[i - 7U] + s1;
  }

  uint32_t a = ctx->state[0];
  uint32_t b = ctx->state[1];
  uint32_t c = ctx->state[2];
  uint32_t d = ctx->state[3];
  uint32_t e = ctx->state[4];
  uint32_t f = ctx->state[5];
  uint32_t g = ctx->state[6];
  uint32_t h = ctx->state[7];

  for (size_t i = 0U; i < 64U; ++i) {
    uint32_t sigma1 = SECURITY_INS(SHA256_RotR)(e, 6U) ^
                      SECURITY_INS(SHA256_RotR)(e, 11U) ^
                      SECURITY_INS(SHA256_RotR)(e, 25U);
    uint32_t ch = (e & f) ^ ((~e) & g);
    uint32_t temp1 = h + sigma1 + ch + k[i] + w[i];

    uint32_t sigma0 = SECURITY_INS(SHA256_RotR)(a, 2U) ^
                      SECURITY_INS(SHA256_RotR)(a, 13U) ^
                      SECURITY_INS(SHA256_RotR)(a, 22U);
    uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
    uint32_t temp2 = sigma0 + maj;

    h = g;
    g = f;
    f = e;
    e = d + temp1;
    d = c;
    c = b;
    b = a;
    a = temp1 + temp2;
  }

  ctx->state[0] += a;
  ctx->state[1] += b;
  ctx->state[2] += c;
  ctx->state[3] += d;
  ctx->state[4] += e;
  ctx->state[5] += f;
  ctx->state[6] += g;
  ctx->state[7] += h;
}

SECURITY_CRYPTO_SHA256_INIT_PROTOTYPE {
  if (!ctx)
    return;
  ctx->state[0] = UINT32_C(0x6A09E667);
  ctx->state[1] = UINT32_C(0xBB67AE85);
  ctx->state[2] = UINT32_C(0x3C6EF372);
  ctx->state[3] = UINT32_C(0xA54FF53A);
  ctx->state[4] = UINT32_C(0x510E527F);
  ctx->state[5] = UINT32_C(0x9B05688C);
  ctx->state[6] = UINT32_C(0x1F83D9AB);
  ctx->state[7] = UINT32_C(0x5BE0CD19);
  ctx->bytes = 0ULL;
  ctx->filled = 0U;
  memset(ctx->buffer, 0, sizeof(ctx->buffer));
}

SECURITY_CRYPTO_SHA256_UPDATE_PROTOTYPE {
  if (!ctx || !data || size == 0U)
    return;

  const uint8_t *bytes = (const uint8_t *)data;
  ctx->bytes += size;

  while (size > 0U) {
    size_t needed = SECURITY_CRYPTO_CONST(SHA256_BLOCK_BYTES) - ctx->filled;
    size_t toCopy = size < needed ? size : needed;
    memcpy(ctx->buffer + ctx->filled, bytes, toCopy);
    ctx->filled += toCopy;
    bytes += toCopy;
    size -= toCopy;

    if (ctx->filled == SECURITY_CRYPTO_CONST(SHA256_BLOCK_BYTES)) {
      SECURITY_INS(SHA256_ProcessBlock)(ctx);
      ctx->filled = 0U;
    }
  }
}

SECURITY_CRYPTO_SHA256_FINAL_PROTOTYPE {
  if (!ctx || !outDigest)
    return STATUS_CONST(INVALID_ARGUMENT);

  uint8_t pad[64] = {0x80U};
  uint8_t lengthBytes[8];
  uint64_t bitCount = ctx->bytes * 8ULL;

  for (size_t i = 0U; i < 8U; ++i)
    lengthBytes[i] = (uint8_t)((bitCount >> (8U * (7U - i))) & 0xFFU);

  size_t padLen = (ctx->filled < 56U) ? (56U - ctx->filled)
                                      : (120U - ctx->filled);
  if (padLen <= 64U) {
    SECURITY_CRYPTO_FUNC(SHA256_Update)(ctx, pad, padLen);
  } else {
    SECURITY_CRYPTO_FUNC(SHA256_Update)(ctx, pad, 64U);
    uint8_t zeros[64] = {0};
    SECURITY_CRYPTO_FUNC(SHA256_Update)(ctx, zeros, padLen - 64U);
  }
  SECURITY_CRYPTO_FUNC(SHA256_Update)(ctx, lengthBytes, 8U);

  uint8_t raw[32];
  for (size_t i = 0U; i < 8U; ++i) {
    raw[i * 4U] = (uint8_t)((ctx->state[i] >> 24U) & 0xFFU);
    raw[(i * 4U) + 1U] = (uint8_t)((ctx->state[i] >> 16U) & 0xFFU);
    raw[(i * 4U) + 2U] = (uint8_t)((ctx->state[i] >> 8U) & 0xFFU);
    raw[(i * 4U) + 3U] = (uint8_t)(ctx->state[i] & 0xFFU);
  }

  return SECURITY_DIGEST_FUNC(Init)(outDigest, raw, sizeof(raw));
}

SECURITY_CRYPTO_SHA256_COMPUTE_PROTOTYPE {
  if (!outDigest)
    return STATUS_CONST(INVALID_ARGUMENT);

  SECURITY_CRYPTO_TYPE(TSHA256) ctx;
  SECURITY_CRYPTO_FUNC(SHA256_Init)(&ctx);
  SECURITY_CRYPTO_FUNC(SHA256_Update)(&ctx, data, size);
  return SECURITY_CRYPTO_FUNC(SHA256_Final)(&ctx, outDigest);
}
/* EOF */
