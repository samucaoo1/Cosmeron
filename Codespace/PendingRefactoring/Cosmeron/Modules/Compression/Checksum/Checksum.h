#pragma once

#include "../Compression.space"
#include "../../Security/Checksum/Checksum.h"

#define COMPRESSION_CRC32_PROTOTYPE                                            \
  static inline uint32_t COMPRESSION_FUNC(CRC32)(                              \
      uint32_t previous, const void *data, size_t size)

#define COMPRESSION_ADLER32_PROTOTYPE                                          \
  static inline uint32_t COMPRESSION_FUNC(Adler32)(                            \
      uint32_t previous, const void *data, size_t size)

COMPRESSION_CRC32_PROTOTYPE {
  return SECURITY_CHECKSUM_FUNC(CRC32_ComputeUInt32)(previous, data, size);
}

COMPRESSION_ADLER32_PROTOTYPE {
  return SECURITY_CHECKSUM_FUNC(Adler32_ComputeUInt32)(previous, data, size);
}
/* EOF */
