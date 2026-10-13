#define COSMERON_NAMESPACE Lab
#define COSMERON_NAMESPACE_CONST LAB
#include "../../Cosmeron/Modules/Compression/Compression.h"
#include <assert.h>
int main(void) {
  Lab_Compression_TBuffer buffer = {0};
  assert(Lab_Compression_Functions.CompressAlloc(LAB_COMPRESSION_ZLIB, "abc", 3,
      LAB_COMPRESSION_LEVEL_DEFAULT, &buffer) == LAB_STATUS_SUCCESS);
  Lab_Compression_BufferFree(&buffer);
  return 0;
}
