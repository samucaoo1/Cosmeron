#include "../../Cosmeron/Modules/Compression/Compression.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv) {
  FILE *f;
  long fileSize;
  unsigned char *bytes;
  Compression_TZipReader reader = {0};
  Compression_TBuffer data = {0};
  size_t len, count;
  OPSTATUS result;
  if (argc != 5) return 2;
  f = fopen(argv[3], "rb");
  if (!f || fseek(f, 0, SEEK_END)) return 3;
  fileSize = ftell(f);
  if (fileSize < 0 || fseek(f, 0, SEEK_SET)) return 4;
  len = (size_t)fileSize;
  bytes = (unsigned char *)malloc(len ? len : 1);
  if (!bytes || fread(bytes, 1, len, f) != len || fclose(f)) return 5;
  result = Compression_ZipOpen(&reader, bytes, len);
  if (!strcmp(argv[1], "rejectzip")) {
    if (result == STATUS_SUCCESS) {
      result = Compression_ZipExtractAlloc(&reader, 0, 100000, &data);
      Compression_BufferFree(&data);
    }
    Compression_ZipClose(&reader);
    free(bytes);
    return result == STATUS_SUCCESS ? 6 : 0;
  }
  if (result != STATUS_SUCCESS ||
      Compression_ZipCount(&reader, &count) != STATUS_SUCCESS || count != 1 ||
      Compression_ZipExtractAlloc(&reader, 0, 100000, &data) != STATUS_SUCCESS) {
    Compression_ZipClose(&reader);
    free(bytes);
    return 7;
  }
  f = fopen(argv[4], "wb");
  if (!f || fwrite(data.data, 1, data.size, f) != data.size || fclose(f))
    return 8;
  Compression_BufferFree(&data);
  Compression_ZipClose(&reader);
  free(bytes);
  return 0;
}
