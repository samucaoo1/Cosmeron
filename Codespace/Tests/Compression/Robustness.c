#include "../../Cosmeron/Modules/Compression/Compression.h"
#include <assert.h>
int main(void) {
  unsigned char bytes[512], output[1024];
  uint32_t state = 42; size_t i, j, outSize;
  for (i = 0; i < 3000; ++i) {
    Compression_TZipReader reader = {0};
    for (j = 0; j < sizeof(bytes); ++j) { state = state * UINT32_C(1664525) + UINT32_C(1013904223); bytes[j] = (unsigned char)(state >> 24); }
    for (j = COMPRESSION_DEFLATE; j <= COMPRESSION_GZIP; ++j) {
      outSize = 999;
      (void)Compression_Decompress((Compression_Format)j, bytes, i % sizeof(bytes), output, sizeof(output), &outSize);
    }
    (void)Compression_ZipOpen(&reader, bytes, i % sizeof(bytes)); Compression_ZipClose(&reader);
  }
  return 0;
}
