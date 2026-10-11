#include "../../Cosmeron/Modules/Compression/Compression.h"
#include <assert.h>
int main(void) {
  unsigned char data[12000], decoded[12000], compressed[14000];
  size_t size, written, i, j, bound;
  int format, level;
  for (i = 0; i < sizeof(data); ++i) data[i] = (unsigned char)(i % 17);
  assert(Compression_CRC32(0, "123456789", 9) == UINT32_C(0xcbf43926));
  assert(Compression_Adler32(1, "123456789", 9) == UINT32_C(0x091e01de));
  assert(Compression_CRC32(Compression_CRC32(0, data, 500), data + 500, 700) == Compression_CRC32(0, data, 1200));
  assert(Compression_Adler32(Compression_Adler32(1, data, 500), data + 500, 700) == Compression_Adler32(1, data, 1200));
  for (format = COMPRESSION_DEFLATE; format <= COMPRESSION_GZIP; ++format) {
    for (level = COMPRESSION_LEVEL_DEFAULT; level <= COMPRESSION_LEVEL_BEST; ++level) {
      Compression_TBuffer packed = {0}, unpacked = {0};
      assert(Compression_Bound((Compression_Format)format, sizeof(data), &bound) == STATUS_SUCCESS);
      assert(bound < sizeof(compressed));
      assert(Compression_Compress((Compression_Format)format, data, sizeof(data), compressed, sizeof(compressed), level, &size) == STATUS_SUCCESS);
      assert(size <= bound);
      assert(Compression_Decompress((Compression_Format)format, compressed, size, decoded, sizeof(decoded), &written) == STATUS_SUCCESS);
      assert(written == sizeof(data) && !memcmp(data, decoded, written));
      written = 777;
      assert(Compression_Decompress((Compression_Format)format, compressed, size, decoded, sizeof(decoded) - 1, &written) == STATUS_INSUFFICIENT_SPACE);
      assert(written == 777);
      assert(Compression_CompressAlloc((Compression_Format)format, data, sizeof(data), level, &packed) == STATUS_SUCCESS);
      assert(Compression_DecompressAlloc((Compression_Format)format, packed.data, packed.size, sizeof(data), &unpacked) == STATUS_SUCCESS);
      assert(unpacked.size == sizeof(data) && !memcmp(unpacked.data, data, sizeof(data)));
      Compression_BufferFree(&unpacked);
      assert(Compression_DecompressAlloc((Compression_Format)format, packed.data, packed.size, sizeof(data) - 1, &unpacked) == STATUS_INSUFFICIENT_SPACE);
      assert(!unpacked.data && !unpacked.size);
      Compression_BufferFree(&packed); Compression_BufferFree(&packed);
      assert(Compression_Compress((Compression_Format)format, NULL, 0, compressed, sizeof(compressed), level, &size) == STATUS_SUCCESS);
      assert(Compression_Decompress((Compression_Format)format, compressed, size, NULL, 0, &written) == STATUS_SUCCESS && written == 0);
      assert(Compression_DecompressAlloc((Compression_Format)format, compressed, size, 0, &unpacked) == STATUS_SUCCESS && unpacked.size == 0);
      Compression_BufferFree(&unpacked);
    }
    assert(Compression_Compress((Compression_Format)format, data, sizeof(data), compressed, sizeof(compressed), 6, &size) == STATUS_SUCCESS);
    for (j = 0; j < size; ++j) {
      written = 777;
      assert(Compression_Decompress((Compression_Format)format, compressed, j, decoded, sizeof(decoded), &written) != STATUS_SUCCESS);
      assert(written == 777);
    }
    compressed[size] = 0;
    assert(Compression_Decompress((Compression_Format)format, compressed, size + 1, decoded, sizeof(decoded), &written) != STATUS_SUCCESS);
    if (format != COMPRESSION_DEFLATE) {
      compressed[size - 1] ^= 0x80;
      assert(Compression_Decompress((Compression_Format)format, compressed, size, decoded, sizeof(decoded), &written) == STATUS_INVALID_SEQUENCE);
    }
  }
  for (i = 0; i < sizeof(data); ++i) data[i] = (unsigned char)((i * 109 + i / 17) & 255);
  assert(Compression_Compress(COMPRESSION_ZLIB, data, sizeof(data), compressed, sizeof(compressed), 9, &size) == STATUS_SUCCESS);
  assert(Compression_Decompress(COMPRESSION_ZLIB, compressed, size, decoded, sizeof(decoded), &written) == STATUS_SUCCESS);
  assert(!memcmp(data, decoded, sizeof(data)));
  written = 777;
  assert(Compression_Compress(COMPRESSION_GZIP, data, 10, compressed, 1, 6, &written) == STATUS_INSUFFICIENT_SPACE && written == 777);
  assert(Compression_Compress(COMPRESSION_ZLIB, data, 10, data, sizeof(data), 6, &written) == STATUS_INVALID_ARGUMENT);
  assert(Compression_Compress((Compression_Format)99, data, 10, compressed, sizeof(compressed), 6, &written) == STATUS_INVALID_ARGUMENT);
  assert(Compression_Compress(COMPRESSION_ZLIB, data, 10, compressed, sizeof(compressed), 10, &written) == STATUS_INVALID_ARGUMENT);
  assert(Compression_Compress(COMPRESSION_ZLIB, NULL, 10, compressed, sizeof(compressed), 6, &written) == STATUS_INVALID_ARGUMENT);
  assert(Compression_Decompress(COMPRESSION_ZLIB, NULL, 1, decoded, sizeof(decoded), &written) == STATUS_INVALID_ARGUMENT);
  assert(Compression_Bound(COMPRESSION_ZLIB, SIZE_MAX, &written) == STATUS_OUT_OF_RANGE);
  assert(Compression_Bound(COMPRESSION_ZLIB, 100, NULL) == STATUS_INVALID_ARGUMENT);
  assert(Compression_Functions.CRC32(0, "123456789", 9) == UINT32_C(0xcbf43926));
  return 0;
}
