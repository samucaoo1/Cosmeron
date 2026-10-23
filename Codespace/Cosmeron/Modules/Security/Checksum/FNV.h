#pragma once

#include "../Digest/Digest.h"

#define SECURITY_CHECKSUM_FNV32_OFFSET_BASIS UINT32_C(2166136261)
#define SECURITY_CHECKSUM_FNV32_PRIME UINT32_C(16777619)

#define SECURITY_CHECKSUM_FNV64_OFFSET_BASIS                                   \
  UINT64_C(14695981039346656037)
#define SECURITY_CHECKSUM_FNV64_PRIME UINT64_C(1099511628211)

typedef struct {
  uint32_t state;
} SECURITY_CHECKSUM_TYPE(TFNV32);

typedef struct {
  uint64_t state;
} SECURITY_CHECKSUM_TYPE(TFNV64);

/* -------------------------------------------------------------------------- */
/* FNV-1 (32-bit)                                                             */
/* -------------------------------------------------------------------------- */

static inline void SECURITY_CHECKSUM_FUNC(FNV1_32_Init)(
    SECURITY_CHECKSUM_TYPE(TFNV32) * ctx) {
  if (ctx)
    ctx->state = SECURITY_CHECKSUM_FNV32_OFFSET_BASIS;
}

static inline void SECURITY_CHECKSUM_FUNC(FNV1_32_Update)(
    SECURITY_CHECKSUM_TYPE(TFNV32) * ctx, const void *data, size_t size) {
  if (!ctx || !data || size == 0U)
    return;
  const uint8_t *bytes = (const uint8_t *)data;
  uint32_t hash = ctx->state;
  for (size_t i = 0U; i < size; ++i) {
    hash *= SECURITY_CHECKSUM_FNV32_PRIME;
    hash ^= bytes[i];
  }
  ctx->state = hash;
}

static inline uint32_t SECURITY_CHECKSUM_FUNC(FNV1_32_FinalUInt32)(
    const SECURITY_CHECKSUM_TYPE(TFNV32) * ctx) {
  return ctx ? ctx->state : 0U;
}

static inline OPSTATUS SECURITY_CHECKSUM_FUNC(FNV1_32_Final)(
    const SECURITY_CHECKSUM_TYPE(TFNV32) * ctx,
    SECURITY_TYPE(TDigest) * outDigest) {
  if (!ctx || !outDigest)
    return STATUS_CONST(INVALID_ARGUMENT);

  uint32_t result = ctx->state;
  uint8_t raw[4];
  raw[0] = (uint8_t)((result >> 24U) & 0xFFU);
  raw[1] = (uint8_t)((result >> 16U) & 0xFFU);
  raw[2] = (uint8_t)((result >> 8U) & 0xFFU);
  raw[3] = (uint8_t)(result & 0xFFU);

  return SECURITY_DIGEST_FUNC(Init)(outDigest, raw, sizeof(raw));
}

static inline OPSTATUS SECURITY_CHECKSUM_FUNC(FNV1_32_Compute)(
    const void *data, size_t size, SECURITY_TYPE(TDigest) *outDigest) {
  SECURITY_CHECKSUM_TYPE(TFNV32) ctx;
  SECURITY_CHECKSUM_FUNC(FNV1_32_Init)(&ctx);
  SECURITY_CHECKSUM_FUNC(FNV1_32_Update)(&ctx, data, size);
  return SECURITY_CHECKSUM_FUNC(FNV1_32_Final)(&ctx, outDigest);
}

/* -------------------------------------------------------------------------- */
/* FNV-1a (32-bit)                                                            */
/* -------------------------------------------------------------------------- */

static inline void SECURITY_CHECKSUM_FUNC(FNV1a_32_Init)(
    SECURITY_CHECKSUM_TYPE(TFNV32) * ctx) {
  if (ctx)
    ctx->state = SECURITY_CHECKSUM_FNV32_OFFSET_BASIS;
}

static inline void SECURITY_CHECKSUM_FUNC(FNV1a_32_Update)(
    SECURITY_CHECKSUM_TYPE(TFNV32) * ctx, const void *data, size_t size) {
  if (!ctx || !data || size == 0U)
    return;
  const uint8_t *bytes = (const uint8_t *)data;
  uint32_t hash = ctx->state;
  for (size_t i = 0U; i < size; ++i) {
    hash ^= bytes[i];
    hash *= SECURITY_CHECKSUM_FNV32_PRIME;
  }
  ctx->state = hash;
}

static inline uint32_t SECURITY_CHECKSUM_FUNC(FNV1a_32_FinalUInt32)(
    const SECURITY_CHECKSUM_TYPE(TFNV32) * ctx) {
  return ctx ? ctx->state : 0U;
}

static inline OPSTATUS SECURITY_CHECKSUM_FUNC(FNV1a_32_Final)(
    const SECURITY_CHECKSUM_TYPE(TFNV32) * ctx,
    SECURITY_TYPE(TDigest) * outDigest) {
  if (!ctx || !outDigest)
    return STATUS_CONST(INVALID_ARGUMENT);

  uint32_t result = ctx->state;
  uint8_t raw[4];
  raw[0] = (uint8_t)((result >> 24U) & 0xFFU);
  raw[1] = (uint8_t)((result >> 16U) & 0xFFU);
  raw[2] = (uint8_t)((result >> 8U) & 0xFFU);
  raw[3] = (uint8_t)(result & 0xFFU);

  return SECURITY_DIGEST_FUNC(Init)(outDigest, raw, sizeof(raw));
}

static inline OPSTATUS SECURITY_CHECKSUM_FUNC(FNV1a_32_Compute)(
    const void *data, size_t size, SECURITY_TYPE(TDigest) *outDigest) {
  SECURITY_CHECKSUM_TYPE(TFNV32) ctx;
  SECURITY_CHECKSUM_FUNC(FNV1a_32_Init)(&ctx);
  SECURITY_CHECKSUM_FUNC(FNV1a_32_Update)(&ctx, data, size);
  return SECURITY_CHECKSUM_FUNC(FNV1a_32_Final)(&ctx, outDigest);
}

/* -------------------------------------------------------------------------- */
/* FNV-1 (64-bit)                                                             */
/* -------------------------------------------------------------------------- */

static inline void SECURITY_CHECKSUM_FUNC(FNV1_64_Init)(
    SECURITY_CHECKSUM_TYPE(TFNV64) * ctx) {
  if (ctx)
    ctx->state = SECURITY_CHECKSUM_FNV64_OFFSET_BASIS;
}

static inline void SECURITY_CHECKSUM_FUNC(FNV1_64_Update)(
    SECURITY_CHECKSUM_TYPE(TFNV64) * ctx, const void *data, size_t size) {
  if (!ctx || !data || size == 0U)
    return;
  const uint8_t *bytes = (const uint8_t *)data;
  uint64_t hash = ctx->state;
  for (size_t i = 0U; i < size; ++i) {
    hash *= SECURITY_CHECKSUM_FNV64_PRIME;
    hash ^= bytes[i];
  }
  ctx->state = hash;
}

static inline uint64_t SECURITY_CHECKSUM_FUNC(FNV1_64_FinalUInt64)(
    const SECURITY_CHECKSUM_TYPE(TFNV64) * ctx) {
  return ctx ? ctx->state : 0ULL;
}

static inline OPSTATUS SECURITY_CHECKSUM_FUNC(FNV1_64_Final)(
    const SECURITY_CHECKSUM_TYPE(TFNV64) * ctx,
    SECURITY_TYPE(TDigest) * outDigest) {
  if (!ctx || !outDigest)
    return STATUS_CONST(INVALID_ARGUMENT);

  uint64_t result = ctx->state;
  uint8_t raw[8];
  raw[0] = (uint8_t)((result >> 56ULL) & 0xFFU);
  raw[1] = (uint8_t)((result >> 48ULL) & 0xFFU);
  raw[2] = (uint8_t)((result >> 40ULL) & 0xFFU);
  raw[3] = (uint8_t)((result >> 32ULL) & 0xFFU);
  raw[4] = (uint8_t)((result >> 24ULL) & 0xFFU);
  raw[5] = (uint8_t)((result >> 16ULL) & 0xFFU);
  raw[6] = (uint8_t)((result >> 8ULL) & 0xFFU);
  raw[7] = (uint8_t)(result & 0xFFU);

  return SECURITY_DIGEST_FUNC(Init)(outDigest, raw, sizeof(raw));
}

static inline OPSTATUS SECURITY_CHECKSUM_FUNC(FNV1_64_Compute)(
    const void *data, size_t size, SECURITY_TYPE(TDigest) *outDigest) {
  SECURITY_CHECKSUM_TYPE(TFNV64) ctx;
  SECURITY_CHECKSUM_FUNC(FNV1_64_Init)(&ctx);
  SECURITY_CHECKSUM_FUNC(FNV1_64_Update)(&ctx, data, size);
  return SECURITY_CHECKSUM_FUNC(FNV1_64_Final)(&ctx, outDigest);
}

/* -------------------------------------------------------------------------- */
/* FNV-1a (64-bit)                                                            */
/* -------------------------------------------------------------------------- */

static inline void SECURITY_CHECKSUM_FUNC(FNV1a_64_Init)(
    SECURITY_CHECKSUM_TYPE(TFNV64) * ctx) {
  if (ctx)
    ctx->state = SECURITY_CHECKSUM_FNV64_OFFSET_BASIS;
}

static inline void SECURITY_CHECKSUM_FUNC(FNV1a_64_Update)(
    SECURITY_CHECKSUM_TYPE(TFNV64) * ctx, const void *data, size_t size) {
  if (!ctx || !data || size == 0U)
    return;
  const uint8_t *bytes = (const uint8_t *)data;
  uint64_t hash = ctx->state;
  for (size_t i = 0U; i < size; ++i) {
    hash ^= bytes[i];
    hash *= SECURITY_CHECKSUM_FNV64_PRIME;
  }
  ctx->state = hash;
}

static inline uint64_t SECURITY_CHECKSUM_FUNC(FNV1a_64_FinalUInt64)(
    const SECURITY_CHECKSUM_TYPE(TFNV64) * ctx) {
  return ctx ? ctx->state : 0ULL;
}

static inline OPSTATUS SECURITY_CHECKSUM_FUNC(FNV1a_64_Final)(
    const SECURITY_CHECKSUM_TYPE(TFNV64) * ctx,
    SECURITY_TYPE(TDigest) * outDigest) {
  if (!ctx || !outDigest)
    return STATUS_CONST(INVALID_ARGUMENT);

  uint64_t result = ctx->state;
  uint8_t raw[8];
  raw[0] = (uint8_t)((result >> 56ULL) & 0xFFU);
  raw[1] = (uint8_t)((result >> 48ULL) & 0xFFU);
  raw[2] = (uint8_t)((result >> 40ULL) & 0xFFU);
  raw[3] = (uint8_t)((result >> 32ULL) & 0xFFU);
  raw[4] = (uint8_t)((result >> 24ULL) & 0xFFU);
  raw[5] = (uint8_t)((result >> 16ULL) & 0xFFU);
  raw[6] = (uint8_t)((result >> 8ULL) & 0xFFU);
  raw[7] = (uint8_t)(result & 0xFFU);

  return SECURITY_DIGEST_FUNC(Init)(outDigest, raw, sizeof(raw));
}

static inline OPSTATUS SECURITY_CHECKSUM_FUNC(FNV1a_64_Compute)(
    const void *data, size_t size, SECURITY_TYPE(TDigest) *outDigest) {
  SECURITY_CHECKSUM_TYPE(TFNV64) ctx;
  SECURITY_CHECKSUM_FUNC(FNV1a_64_Init)(&ctx);
  SECURITY_CHECKSUM_FUNC(FNV1a_64_Update)(&ctx, data, size);
  return SECURITY_CHECKSUM_FUNC(FNV1a_64_Final)(&ctx, outDigest);
}
/* EOF */
