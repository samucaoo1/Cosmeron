#include "../../Cosmeron/Modules/Compression/Compression.h"
#include <assert.h>
#include <string.h>

int main(void) {
  unsigned char input[70000], packed[70100], output[70000];
  size_t i, used = 999, written = 999;
  int format, level;
  for (i = 0; i < sizeof input; ++i)
    input[i] = (unsigned char)((i * 97U + i / 251U) & 255U);
  assert(Compression_CRC32(0, "123456789", 9) == UINT32_C(0xcbf43926));
  assert(Compression_Adler32(1, "123456789", 9) == UINT32_C(0x091e01de));
  for (format = COMPRESSION_DEFLATE; format <= COMPRESSION_GZIP; ++format) {
    for (level = COMPRESSION_LEVEL_DEFAULT; level <= COMPRESSION_LEVEL_BEST; ++level) {
      Compression_TBuffer compressed = {0}, decompressed = {0};
      assert(Compression_Compress((Compression_Format)format,
          input, sizeof input, packed, sizeof packed, level, &used) == STATUS_SUCCESS);
      assert(Compression_Decompress((Compression_Format)format,
          packed, used, output, sizeof output, &written) == STATUS_SUCCESS);
      assert(written == sizeof input && !memcmp(input, output, written));
      written = 999;
      assert(Compression_Decompress((Compression_Format)format,
          packed, used, output, sizeof output - 1, &written) == STATUS_INSUFFICIENT_SPACE);
      assert(written == 999);
      assert(Compression_CompressAlloc((Compression_Format)format,
          input, sizeof input, level, &compressed) == STATUS_SUCCESS);
      assert(Compression_DecompressAlloc((Compression_Format)format, compressed.data,
          compressed.size, sizeof input, &decompressed) == STATUS_SUCCESS);
      assert(decompressed.size == sizeof input &&
          !memcmp(decompressed.data, input, sizeof input));
      Compression_BufferFree(&decompressed);
      Compression_BufferFree(&compressed);
      assert(Compression_Compress((Compression_Format)format,
          NULL, 0, packed, sizeof packed, level, &used) == STATUS_SUCCESS);
      assert(Compression_Decompress((Compression_Format)format,
          packed, used, NULL, 0, &written) == STATUS_SUCCESS && written == 0);
    }
    assert(Compression_Compress((Compression_Format)format, input, 12, packed,
        sizeof packed, 6, &used) == STATUS_SUCCESS);
    packed[used] = 0;
    assert(Compression_Decompress((Compression_Format)format, packed, used + 1,
        output, sizeof output, &written) != STATUS_SUCCESS);
    assert(Compression_Decompress((Compression_Format)format, packed, used - 1,
        output, sizeof output, &written) != STATUS_SUCCESS);
    if (format != COMPRESSION_DEFLATE) {
      packed[used - 1] ^= 0x80;
      assert(Compression_Decompress((Compression_Format)format, packed, used,
          output, sizeof output, &written) == STATUS_INVALID_SEQUENCE);
    }
  }
  {
    unsigned char repeated[8192];
    size_t stored = 0, reduced = 0;
    memset(repeated, 'X', sizeof repeated);
    assert(Compression_Compress(COMPRESSION_ZLIB, repeated, sizeof repeated,
        packed, sizeof packed, COMPRESSION_LEVEL_STORE, &stored) == STATUS_SUCCESS);
    assert(Compression_Compress(COMPRESSION_ZLIB, repeated, sizeof repeated,
        packed, sizeof packed, COMPRESSION_LEVEL_BEST, &reduced) == STATUS_SUCCESS);
    assert(reduced < stored / 10);
    assert(Compression_Decompress(COMPRESSION_ZLIB, packed, reduced,
        output, sizeof output, &written) == STATUS_SUCCESS);
    assert(written == sizeof repeated && !memcmp(repeated, output, written));
  }
  {
    unsigned char skewed[16384];
    uint32_t state = UINT32_C(0x12345abc);
    size_t compressedSize = 0;
    for (i = 0; i < sizeof skewed; ++i) {
      state ^= state << 13;
      state ^= state >> 17;
      state ^= state << 5;
      skewed[i] = (unsigned char)('a' + ((state >> 24) & 7U));
    }
    assert(Compression_Compress(COMPRESSION_DEFLATE, skewed, sizeof skewed,
        packed, sizeof packed, COMPRESSION_LEVEL_BEST, &compressedSize) == STATUS_SUCCESS);
    assert((packed[0] & 7U) == 5U);
    assert(Compression_Decompress(COMPRESSION_DEFLATE, packed, compressedSize,
        output, sizeof output, &written) == STATUS_SUCCESS);
    assert(written == sizeof skewed && !memcmp(skewed, output, written));
  }
  return 0;
}
