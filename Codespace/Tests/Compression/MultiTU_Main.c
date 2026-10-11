#include "../../Cosmeron/Modules/Compression/Compression.h"
#include <assert.h>
OPSTATUS pack(Compression_TBuffer *out);
void unpack(Compression_TBuffer *in);
int main(void) {
  Compression_TBuffer buffer = {0};
  assert(pack(&buffer) == STATUS_SUCCESS); unpack(&buffer);
  return buffer.data != NULL;
}
