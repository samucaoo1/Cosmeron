#pragma once
#include "../Compression.space"
#define COMPRESSION_CRC32_PROTOTYPE \
  static inline uint32_t COMPRESSION_FUNC(CRC32)( \
      uint32_t previous, const void *data, size_t size)
#define COMPRESSION_ADLER32_PROTOTYPE \
  static inline uint32_t COMPRESSION_FUNC(Adler32)( \
      uint32_t previous, const void *data, size_t size)
COMPRESSION_CRC32_PROTOTYPE {
  return (uint32_t)cosmeron_compression_vendor_mz_crc32(previous, (const unsigned char *)data, size);
}
COMPRESSION_ADLER32_PROTOTYPE {
  return (uint32_t)cosmeron_compression_vendor_mz_adler32(previous, (const unsigned char *)data, size);
}
