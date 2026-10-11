#include "../../Cosmeron/Modules/Compression/Compression.h"
#include <assert.h>
int main(void) {
  const char *original = "compression-original.tmp", *packed = "compression-packed.tmp", *restored = "compression-restored.tmp";
  void *data = NULL; size_t size = 0;
  assert(File_Save(original, "Cosmeron gzip file", 18) == STATUS_SUCCESS);
  assert(File_Save(restored, "preserve", 8) == STATUS_SUCCESS);
  assert(Compression_CompressFile(COMPRESSION_GZIP, original, packed, 6, 17) == STATUS_OUT_OF_RANGE);
  assert(Compression_CompressFile(COMPRESSION_GZIP, original, packed, 6, 18) == STATUS_SUCCESS);
  assert(Compression_DecompressFile(COMPRESSION_GZIP, packed, restored, 1024, 17) == STATUS_INSUFFICIENT_SPACE);
  assert(File_Load(restored, &data, &size) == STATUS_SUCCESS && size == 8 && !memcmp(data,"preserve",8)); free(data);
  assert(Compression_DecompressFile(COMPRESSION_GZIP, packed, restored, 1024, 18) == STATUS_SUCCESS);
  assert(File_Load(restored, &data, &size) == STATUS_SUCCESS && size == 18 && !memcmp(data,"Cosmeron gzip file",18)); free(data);
  assert(Compression_CompressFile(COMPRESSION_GZIP, original, original, 6, 18) == STATUS_SUCCESS);
  assert(Compression_DecompressFile(COMPRESSION_GZIP, original, original, 1024, 18) == STATUS_SUCCESS);
  assert(File_Remove(original) == STATUS_SUCCESS && File_Remove(packed) == STATUS_SUCCESS && File_Remove(restored) == STATUS_SUCCESS);
  return 0;
}
