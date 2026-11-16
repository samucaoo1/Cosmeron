#pragma once

#include "../Digest/Digest.h"

enum {
  SECURITY_CRYPTO_CONST(MD5_DIGEST_BYTES) = 16U,
  SECURITY_CRYPTO_CONST(MD5_BLOCK_BYTES) = 64U
};

typedef struct {
  uint32_t state[4];
  uint64_t bytes;
  uint8_t buffer[SECURITY_CRYPTO_CONST(MD5_BLOCK_BYTES)];
  size_t filled;
} SECURITY_CRYPTO_TYPE(TMD5);

#define SECURITY_CRYPTO_MD5_INIT_PROTOTYPE                                     \
  static inline void SECURITY_CRYPTO_FUNC(MD5_Init)(                           \
      SECURITY_CRYPTO_TYPE(TMD5) * ctx)

#define SECURITY_CRYPTO_MD5_UPDATE_PROTOTYPE                                   \
  static inline void SECURITY_CRYPTO_FUNC(MD5_Update)(                         \
      SECURITY_CRYPTO_TYPE(TMD5) * ctx, const void *data, size_t size)

#define SECURITY_CRYPTO_MD5_FINAL_PROTOTYPE                                    \
  static inline OPSTATUS SECURITY_CRYPTO_FUNC(MD5_Final)(                      \
      SECURITY_CRYPTO_TYPE(TMD5) * ctx, SECURITY_TYPE(TDigest) * outDigest)

#define SECURITY_CRYPTO_MD5_COMPUTE_PROTOTYPE                                  \
  static inline OPSTATUS SECURITY_CRYPTO_FUNC(MD5_Compute)(                    \
      const void *data, size_t size, SECURITY_TYPE(TDigest) *outDigest)

static inline void SECURITY_INS(MD5_ProcessBlock)(
    SECURITY_CRYPTO_TYPE(TMD5) * ctx) {
  static const uint32_t k[64] = {
      0xD76AA478U, 0xE8C7B756U, 0x242070DBU, 0xC1BDCEEEU, 0xF57C0FAFU,
      0x4787C62AU, 0xA8304613U, 0xFD469501U, 0x698098D8U, 0x8B44F7AFU,
      0xFFFF5BB1U, 0x895CD7BEU, 0x6B901122U, 0xFD987193U, 0xA679438EU,
      0x49B40821U, 0xF61E2562U, 0xC040B340U, 0x265E5A51U, 0xE9B6C7AAU,
      0xD62F105DU, 0x02441453U, 0xD8A1E681U, 0xE7D3FBC8U, 0x21E1CDE6U,
      0xC33707D6U, 0xF4D50D87U, 0x455A14EDU, 0xA9E3E905U, 0xFCEFA3F8U,
      0x676F02D9U, 0x8D2A4C8AU, 0xFFFA3942U, 0x8771F681U, 0x6D9D6122U,
      0xFDE5380CU, 0xA4BEEA44U, 0x4BDECFA9U, 0xF6BB4B60U, 0xBEBFBC70U,
      0x289B7EC6U, 0xEAA127FAU, 0xD4EF3085U, 0x04881D05U, 0xD9D4D039U,
      0xE6DB99E5U, 0x1FA27CF8U, 0xC4AC5665U, 0xF4292244U, 0x432AFF97U,
      0xAB9423A7U, 0xFC93A039U, 0x655B59C3U, 0x8F0CCC92U, 0xFFEFF47DU,
      0x85845DD1U, 0x6FA87E4FU, 0xFE2CE6E0U, 0xA3014314U, 0x4E0811A1U,
      0xF7537E82U, 0xBD3AF235U, 0x2AD7D2BBU, 0xEB86D391U};

  static const uint8_t shift[64] = {
      7U, 12U, 17U, 22U, 7U, 12U, 17U, 22U, 7U, 12U, 17U, 22U, 7U, 12U, 17U,
      22U, 5U,  9U,  14U, 20U, 5U, 9U,  14U, 20U, 5U, 9U,  14U, 20U, 5U, 9U,
      14U, 20U, 4U,  11U, 16U, 23U, 4U, 11U, 16U, 23U, 4U, 11U, 16U, 23U, 4U,
      11U, 16U, 23U, 6U,  10U, 15U, 21U, 6U, 10U, 15U, 21U, 6U, 10U, 15U, 21U,
      6U,  10U, 15U, 21U};

  uint32_t words[16];
  for (size_t i = 0U; i < 16U; ++i) {
    const uint8_t *p = ctx->buffer + (4U * i);
    words[i] = (uint32_t)p[0] | ((uint32_t)p[1] << 8U) |
               ((uint32_t)p[2] << 16U) | ((uint32_t)p[3] << 24U);
  }

  uint32_t a = ctx->state[0];
  uint32_t b = ctx->state[1];
  uint32_t c = ctx->state[2];
  uint32_t d = ctx->state[3];

  for (size_t i = 0U; i < 64U; ++i) {
    uint32_t f;
    size_t g;
    if (i < 16U) {
      f = (b & c) | ((~b) & d);
      g = i;
    } else if (i < 32U) {
      f = (d & b) | ((~d) & c);
      g = ((5U * i) + 1U) % 16U;
    } else if (i < 48U) {
      f = b ^ c ^ d;
      g = ((3U * i) + 5U) % 16U;
    } else {
      f = c ^ (b | (~d));
      g = (7U * i) % 16U;
    }

    uint32_t temp = a + f + k[i] + words[g];
    a = d;
    d = c;
    c = b;
    b += (temp << shift[i]) | (temp >> (32U - shift[i]));
  }

  ctx->state[0] += a;
  ctx->state[1] += b;
  ctx->state[2] += c;
  ctx->state[3] += d;
}

SECURITY_CRYPTO_MD5_INIT_PROTOTYPE {
  if (!ctx)
    return;
  ctx->state[0] = UINT32_C(0x67452301);
  ctx->state[1] = UINT32_C(0xEFCDAB89);
  ctx->state[2] = UINT32_C(0x98BADCFE);
  ctx->state[3] = UINT32_C(0x10325476);
  ctx->bytes = 0ULL;
  ctx->filled = 0U;
  memset(ctx->buffer, 0, sizeof(ctx->buffer));
}

SECURITY_CRYPTO_MD5_UPDATE_PROTOTYPE {
  if (!ctx || !data || size == 0U)
    return;

  const uint8_t *bytes = (const uint8_t *)data;
  ctx->bytes += size;

  while (size > 0U) {
    size_t needed = SECURITY_CRYPTO_CONST(MD5_BLOCK_BYTES) - ctx->filled;
    size_t toCopy = size < needed ? size : needed;
    memcpy(ctx->buffer + ctx->filled, bytes, toCopy);
    ctx->filled += toCopy;
    bytes += toCopy;
    size -= toCopy;

    if (ctx->filled == SECURITY_CRYPTO_CONST(MD5_BLOCK_BYTES)) {
      SECURITY_INS(MD5_ProcessBlock)(ctx);
      ctx->filled = 0U;
    }
  }
}

SECURITY_CRYPTO_MD5_FINAL_PROTOTYPE {
  if (!ctx || !outDigest)
    return STATUS_CONST(INVALID_ARGUMENT);

  uint8_t pad[64] = {0x80U};
  uint8_t lengthBytes[8];
  uint64_t bitCount = ctx->bytes * 8ULL;

  for (size_t i = 0U; i < 8U; ++i)
    lengthBytes[i] = (uint8_t)((bitCount >> (8U * i)) & 0xFFU);

  size_t padLen = (ctx->filled < 56U) ? (56U - ctx->filled)
                                      : (120U - ctx->filled);
  if (padLen <= 64U) {
    SECURITY_CRYPTO_FUNC(MD5_Update)(ctx, pad, padLen);
  } else {
    SECURITY_CRYPTO_FUNC(MD5_Update)(ctx, pad, 64U);
    uint8_t zeros[64] = {0};
    SECURITY_CRYPTO_FUNC(MD5_Update)(ctx, zeros, padLen - 64U);
  }
  SECURITY_CRYPTO_FUNC(MD5_Update)(ctx, lengthBytes, 8U);

  uint8_t raw[16];
  for (size_t i = 0U; i < 16U; ++i)
    raw[i] = (uint8_t)((ctx->state[i / 4U] >> (8U * (i % 4U))) & 0xFFU);

  return SECURITY_DIGEST_FUNC(Init)(outDigest, raw, sizeof(raw));
}

SECURITY_CRYPTO_MD5_COMPUTE_PROTOTYPE {
  if (!outDigest)
    return STATUS_CONST(INVALID_ARGUMENT);

  SECURITY_CRYPTO_TYPE(TMD5) ctx;
  SECURITY_CRYPTO_FUNC(MD5_Init)(&ctx);
  SECURITY_CRYPTO_FUNC(MD5_Update)(&ctx, data, size);
  return SECURITY_CRYPTO_FUNC(MD5_Final)(&ctx, outDigest);
}
/* EOF */
