#include "../../Cosmeron/Modules/Compression/Compression.h"
#include <assert.h>
void unpack(Compression_TBuffer *in) {
  Compression_TBuffer out = {0};
  assert(Compression_DecompressAlloc(COMPRESSION_ZLIB, in->data, in->size, 8, &out) == STATUS_SUCCESS);
  assert(out.size == 8 && !memcmp(out.data, "cross TU", 8));
  Compression_BufferFree(&out); Compression_BufferFree(in);
}
