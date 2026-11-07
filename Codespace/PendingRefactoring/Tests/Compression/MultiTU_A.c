#include "../../Cosmeron/Modules/Compression/Compression.h"
OPSTATUS pack(Compression_TBuffer *out) {
  return Compression_CompressAlloc(COMPRESSION_ZLIB, "cross TU", 8, -1, out);
}
