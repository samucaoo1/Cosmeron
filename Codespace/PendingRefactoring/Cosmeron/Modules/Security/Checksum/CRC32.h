#pragma once

#include "../Digest/Digest.h"

#define SECURITY_CHECKSUM_CRC32_POLYNOMIAL UINT32_C(0xEDB88320)

typedef struct {
  uint32_t state;
} SECURITY_CHECKSUM_TYPE(TCRC32);

#define SECURITY_CHECKSUM_CRC32_INIT_PROTOTYPE                                 \
  static inline void SECURITY_CHECKSUM_FUNC(CRC32_Init)(                       \
      SECURITY_CHECKSUM_TYPE(TCRC32) * ctx)

#define SECURITY_CHECKSUM_CRC32_UPDATE_PROTOTYPE                               \
  static inline void SECURITY_CHECKSUM_FUNC(CRC32_Update)(                     \
      SECURITY_CHECKSUM_TYPE(TCRC32) * ctx, const void *data, size_t size)

#define SECURITY_CHECKSUM_CRC32_FINAL_PROTOTYPE                                \
  static inline OPSTATUS SECURITY_CHECKSUM_FUNC(CRC32_Final)(                  \
      SECURITY_CHECKSUM_TYPE(TCRC32) * ctx,                                    \
      SECURITY_TYPE(TDigest) * outDigest)

#define SECURITY_CHECKSUM_CRC32_FINAL_UINT32_PROTOTYPE                         \
  static inline uint32_t SECURITY_CHECKSUM_FUNC(CRC32_FinalUInt32)(            \
      SECURITY_CHECKSUM_TYPE(TCRC32) * ctx)

#define SECURITY_CHECKSUM_CRC32_COMPUTE_PROTOTYPE                              \
  static inline OPSTATUS SECURITY_CHECKSUM_FUNC(CRC32_Compute)(                \
      const void *data, size_t size, SECURITY_TYPE(TDigest) *outDigest)

#define SECURITY_CHECKSUM_CRC32_COMPUTE_UINT32_PROTOTYPE                       \
  static inline uint32_t SECURITY_CHECKSUM_FUNC(CRC32_ComputeUInt32)(          \
      uint32_t previous, const void *data, size_t size)

SECURITY_CHECKSUM_CRC32_INIT_PROTOTYPE {
  if (ctx)
    ctx->state = UINT32_C(0xFFFFFFFF);
}

SECURITY_CHECKSUM_CRC32_UPDATE_PROTOTYPE {
  if (!ctx || !data || size == 0U)
    return;
  const uint8_t *bytes = (const uint8_t *)data;
  uint32_t crc = ctx->state;
  for (size_t i = 0U; i < size; ++i) {
    crc ^= bytes[i];
    for (unsigned int bit = 0; bit < 8; ++bit)
      crc = (crc >> 1U) ^
            (SECURITY_CHECKSUM_CRC32_POLYNOMIAL & (0U - (crc & 1U)));
  }
  ctx->state = crc;
}

SECURITY_CHECKSUM_CRC32_FINAL_UINT32_PROTOTYPE {
  return ctx ? ~ctx->state : 0U;
}

SECURITY_CHECKSUM_CRC32_FINAL_PROTOTYPE {
  if (!ctx || !outDigest)
    return STATUS_CONST(INVALID_ARGUMENT);

  uint32_t result = ~ctx->state;
  uint8_t raw[4];
  raw[0] = (uint8_t)((result >> 24U) & 0xFFU);
  raw[1] = (uint8_t)((result >> 16U) & 0xFFU);
  raw[2] = (uint8_t)((result >> 8U) & 0xFFU);
  raw[3] = (uint8_t)(result & 0xFFU);

  return SECURITY_DIGEST_FUNC(Init)(outDigest, raw, sizeof(raw));
}

SECURITY_CHECKSUM_CRC32_COMPUTE_PROTOTYPE {
  if (!outDigest)
    return STATUS_CONST(INVALID_ARGUMENT);

  SECURITY_CHECKSUM_TYPE(TCRC32) ctx;
  SECURITY_CHECKSUM_FUNC(CRC32_Init)(&ctx);
  SECURITY_CHECKSUM_FUNC(CRC32_Update)(&ctx, data, size);
  return SECURITY_CHECKSUM_FUNC(CRC32_Final)(&ctx, outDigest);
}

SECURITY_CHECKSUM_CRC32_COMPUTE_UINT32_PROTOTYPE {
  const uint8_t *bytes = (const uint8_t *)data;
  if (!bytes || size == 0U)
    return previous;

  uint32_t crc = ~previous;
  for (size_t i = 0U; i < size; ++i) {
    crc ^= bytes[i];
    for (unsigned int bit = 0; bit < 8; ++bit)
      crc = (crc >> 1U) ^
            (SECURITY_CHECKSUM_CRC32_POLYNOMIAL & (0U - (crc & 1U)));
  }
  return ~crc;
}
/* EOF */
