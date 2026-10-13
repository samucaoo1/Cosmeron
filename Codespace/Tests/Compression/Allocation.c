#include <stdlib.h>
#include <assert.h>
static size_t calls, failAt, live;
static void *test_malloc(size_t size) {
  void *p; if (++calls == failAt) return NULL;
  p = malloc(size); if (p) ++live; return p;
}
static void *test_calloc(size_t count, size_t size) {
  void *p; if (++calls == failAt) return NULL;
  p = calloc(count,size); if (p) ++live; return p;
}
static void *test_realloc(void *p, size_t size) {
  void *result; if (++calls == failAt) return NULL;
  result = realloc(p,size); if (result && !p) ++live; return result;
}
static void test_free(void *p) { if (p) { assert(live); --live; } free(p); }
#define malloc test_malloc
#define calloc test_calloc
#define realloc test_realloc
#define free test_free
#include "../../Cosmeron/Modules/Compression/Compression.h"
#undef malloc
#undef calloc
#undef realloc
#undef free
int main(void) {
  size_t i; unsigned char compressed[128]; size_t size;
  calls = 0; failAt = 0;
  assert(Compression_Compress(COMPRESSION_GZIP, "allocation",10, compressed,sizeof(compressed),6,&size) == STATUS_SUCCESS);
  assert(!live);
  for (i = 1; i < 20; ++i) {
    Compression_TBuffer buffer = {0}; OPSTATUS status;
    Compression_TZipWriter writer = {0}; Compression_TZipReader reader = {0};
    calls = 0; failAt = i;
    status = Compression_CompressAlloc(COMPRESSION_ZLIB,"allocation",10,6,&buffer);
    assert(status == STATUS_SUCCESS || status == STATUS_OUT_OF_MEMORY);
    if (status != STATUS_SUCCESS) assert(!buffer.data && !buffer.size);
    Compression_BufferFree(&buffer); assert(!live);
    calls = 0;
    status = Compression_DecompressAlloc(COMPRESSION_GZIP,compressed,size,10,&buffer);
    assert(status == STATUS_SUCCESS || status == STATUS_OUT_OF_MEMORY);
    Compression_BufferFree(&buffer); assert(!live);
    calls = 0;
    status = Compression_ZipCreate(&writer);
    if (status == STATUS_SUCCESS) {
      status = Compression_ZipAdd(&writer,"entry","payload",7,6);
      if (status == STATUS_SUCCESS) status = Compression_ZipFinish(&writer,&buffer);
    }
    if (status != STATUS_SUCCESS) { assert(!buffer.data); Compression_ZipDiscard(&writer); }
    if (buffer.data) {
      calls = 0;
      status = Compression_ZipOpen(&reader,buffer.data,buffer.size);
      if (status == STATUS_SUCCESS) {
        Compression_TBuffer output = {0};
        status = Compression_ZipExtractAlloc(&reader,0,7,&output);
        Compression_BufferFree(&output);
      }
      Compression_ZipClose(&reader);
    }
    Compression_BufferFree(&buffer); assert(!live);
  }
  return 0;
}
