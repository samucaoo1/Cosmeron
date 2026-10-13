#include "../../Cosmeron/Modules/Compression/Compression.h"
#include <assert.h>
#include <stdio.h>
static unsigned char *load(const char *path, size_t *size) {
  FILE *file = fopen(path, "rb"); long length; unsigned char *data;
  assert(file && !fseek(file, 0, SEEK_END)); length = ftell(file); assert(length >= 0);
  rewind(file); *size = (size_t)length; data = malloc(*size ? *size : 1); assert(data);
  assert(fread(data, 1, *size, file) == *size); assert(!fclose(file)); return data;
}
static void save(const char *path, const void *data, size_t size) {
  FILE *file = fopen(path, "wb"); assert(file);
  assert(fwrite(data, 1, size, file) == size); assert(!fclose(file));
}
int main(int argc, char **argv) {
  size_t size, i; unsigned char *data;
  Compression_TBuffer out = {0}; OPSTATUS status = STATUS_SUCCESS;
  assert(argc == 5); data = load(argv[3], &size);
  if (!strcmp(argv[1], "encode")) {
    status = Compression_CompressAlloc((Compression_Format)atoi(argv[2]), data, size, 6, &out);
  } else if (!strcmp(argv[1], "decode") || !strcmp(argv[1], "reject")) {
    status = Compression_DecompressAlloc((Compression_Format)atoi(argv[2]), data, size, 1048576, &out);
    if (!strcmp(argv[1], "reject")) { assert(status != STATUS_SUCCESS && !out.data); free(data); return 0; }
  } else if (!strcmp(argv[1], "zipwrite")) {
    Compression_TZipWriter writer = {0};
    assert(Compression_ZipCreate(&writer) == STATUS_SUCCESS);
    assert(Compression_ZipAdd(&writer, "payload.bin", data, size, 6) == STATUS_SUCCESS);
    assert(Compression_ZipAdd(&writer, "empty", NULL, 0, 0) == STATUS_SUCCESS);
    status = Compression_ZipFinish(&writer, &out);
  } else if (!strcmp(argv[1], "zipwrite64")) {
    Compression_TZipWriter writer = {0};
    char filename[32];
    assert(Compression_ZipCreate(&writer) == STATUS_SUCCESS);
    for (i = 0; i < 65536; ++i) {
      assert(snprintf(filename, sizeof filename, "entry-%05u", (unsigned)i) > 0);
      assert(Compression_ZipAdd(&writer, filename, NULL, 0, 0) == STATUS_SUCCESS);
    }
    status = Compression_ZipFinish(&writer, &out);
  } else if (!strcmp(argv[1], "zipread")) {
    Compression_TZipReader reader = {0}; size_t count; Compression_TZipEntry entry;
    assert(Compression_ZipOpen(&reader, data, size) == STATUS_SUCCESS);
    assert(Compression_ZipCount(&reader, &count) == STATUS_SUCCESS && count);
    for (i = 0; i < count; ++i) {
      assert(Compression_ZipEntry(&reader, i, &entry) == STATUS_SUCCESS);
      if (entry.directory) continue;
      status = Compression_ZipExtractAlloc(&reader, i, 1048576, &out);
      if (!entry.supported) { assert(status == STATUS_NOT_SUPPORTED); continue; }
      if (!strcmp(argv[2], "corrupt")) { assert(status != STATUS_SUCCESS); continue; }
      assert(status == STATUS_SUCCESS); save(argv[4], out.data, out.size); Compression_BufferFree(&out);
    }
    Compression_ZipClose(&reader); free(data); return 0;
  } else assert(0);
  assert(status == STATUS_SUCCESS); save(argv[4], out.data, out.size);
  Compression_BufferFree(&out); free(data); return 0;
}
