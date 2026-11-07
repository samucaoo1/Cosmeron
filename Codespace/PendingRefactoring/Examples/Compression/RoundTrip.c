#include "../../Cosmeron/Modules/Compression/Compression.h"
#include <stdio.h>
int main(void) {
  const char text[] = "Cosmeron: a lib for mads scientists.";
  Compression_TBuffer packed = {0}, unpacked = {0};
  OPSTATUS status = Compression_CompressAlloc(COMPRESSION_GZIP, text,
      sizeof(text) - 1, COMPRESSION_LEVEL_DEFAULT, &packed);
  if (status == STATUS_SUCCESS)
    status = Compression_Functions.DecompressAlloc(COMPRESSION_GZIP,
        packed.data, packed.size, sizeof(text) - 1, &unpacked);
  if (status == STATUS_SUCCESS)
    printf("%zu -> %zu bytes; restored: %.*s\n", sizeof(text) - 1, packed.size,
        (int)unpacked.size, (const char *)unpacked.data);
  Compression_BufferFree(&unpacked);
  Compression_BufferFree(&packed);
  return status == STATUS_SUCCESS ? 0 : 1;
}
