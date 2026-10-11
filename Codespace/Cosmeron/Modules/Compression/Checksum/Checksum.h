#pragma once
#include "../Compression.space"

#define COMPRESSION_CRC32_PROTOTYPE \
  static inline uint32_t COMPRESSION_FUNC(CRC32)( \
      uint32_t previous, const void *data, size_t size)
#define COMPRESSION_ADLER32_PROTOTYPE \
  static inline uint32_t COMPRESSION_FUNC(Adler32)( \
      uint32_t previous, const void *data, size_t size)

COMPRESSION_CRC32_PROTOTYPE {
  const unsigned char *bytes = (const unsigned char *)data;
  uint32_t crc = ~previous;
  size_t i;
  if (!bytes) return previous;
  for (i = 0; i < size; ++i) {
    unsigned int bit;
    crc ^= bytes[i];
    for (bit = 0; bit < 8; ++bit)
      crc = (crc >> 1) ^ (UINT32_C(0xedb88320) & (0U - (crc & 1U)));
  }
  return ~crc;
}

COMPRESSION_ADLER32_PROTOTYPE {
  const unsigned char *bytes = (const unsigned char *)data;
  uint32_t a = previous & UINT32_C(65535);
  uint32_t b = previous >> 16;
  if (!bytes) return previous;
  while (size) {
    size_t i, count = size > 5552 ? 5552 : size;
    for (i = 0; i < count; ++i) {
      a += bytes[i];
      b += a;
    }
    a %= UINT32_C(65521);
    b %= UINT32_C(65521);
    bytes += count;
    size -= count;
  }
  return (b << 16) | a;
}
