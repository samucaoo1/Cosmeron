/*
 * Independently generated ZIPs, not produced by Cosmeron/Miniz.
 * Reproduce with Python 3:
 *
 *   info = zipfile.ZipInfo("payload.bin", (1980,1,1,0,0,0))
 *   with zipfile.ZipFile(io.BytesIO(), "w") as z:
 *       z.writestr(info, payload, compress_type=zipfile.ZIP_STORED)
 *   # Repeat with ZIP_DEFLATED. CRC32: 0xe036cc05.
 *
 * Both fixtures have exactly one entry, 'payload.bin'; binary data and
 * embedded zero/non-ASCII bytes exercise byte-for-byte interoperability.
 * No Python/zip executable is required on any BSD CI guest.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char stored[] =
    "504b03041400000000000000210005cc36e0bc000000bc0000000b0000007061796c6f61642e"
    "62696e436f736d65726f6e2065787465726e616c205a495020636f6d7061746962696c697479"
    "20666978747572652e0a436f736d65726f6e2065787465726e616c205a495020636f6d706174"
    "6962696c69747920666978747572652e0a436f736d65726f6e2065787465726e616c205a4950"
    "20636f6d7061746962696c69747920666978747572652e0a436f736d65726f6e206578746572"
    "6e616c205a495020636f6d7061746962696c69747920666978747572652e0a00017f80feff0d"
    "0a504b010214031400000000000000210005cc36e0bc000000bc0000000b0000000000000000"
    "0000008001000000007061796c6f61642e62696e504b0506000000000100010039000000e500"
    "00000000";
static const char deflated[] =
    "504b03041400000008000000210005cc36e03b000000bc0000000b0000007061796c6f61642e"
    "62696e73ce2fce4d2dcacf5348ad28492dca4bcc5188f20c5048cecf2d482cc94ccaccc92ca9"
    "5448cbac28292d4ad5e3721e1c8a1918eb1bfefde7e50200504b010214031400000008000000"
    "210005cc36e03b000000bc0000000b00000000000000000000008001000000007061796c6f61"
    "642e62696e504b0506000000000100010039000000640000000000";
static const char payloadLine[] =
    "Cosmeron external ZIP compatibility fixture.\n";
static const unsigned char payloadEnd[] = {0, 1, 127, 128, 254, 255, '\r', '\n'};

static int digit(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return -1;
}
static int writeHex(const char *directory, const char *name, const char *hex) {
  char path[1024];
  size_t i, size = strlen(hex);
  FILE *out;
  if (snprintf(path, sizeof path, "%s/%s", directory, name) >= (int)sizeof path)
    return 1;
  if (size % 2) return 2;
  out = fopen(path, "wb");
  if (!out) return 3;
  for (i = 0; i < size; i += 2) {
    int a = digit(hex[i]), b = digit(hex[i+1]);
    if (a < 0 || b < 0 || fputc(a*16+b, out) == EOF) {
      fclose(out);
      return 4;
    }
  }
  return fclose(out) == 0 ? 0 : 5;
}
static int writePayload(const char *directory) {
  char path[1024];
  FILE *out;
  int i;
  if (snprintf(path, sizeof path, "%s/external-expected.bin",
               directory) >= (int)sizeof path)
    return 1;
  out = fopen(path, "wb");
  if (!out) return 2;
  for (i = 0; i < 4; ++i) {
    size_t n = sizeof payloadLine - 1;
    if (fwrite(payloadLine, 1, n, out) != n) {
      fclose(out);
      return 3;
    }
  }
  if (fwrite(payloadEnd, 1, sizeof payloadEnd, out) != sizeof payloadEnd) {
    fclose(out);
    return 4;
  }
  return fclose(out) == 0 ? 0 : 5;
}
int main(int argc, char **argv) {
  if (argc != 2) return 2;
  if (writeHex(argv[1], "python-stored.zip", stored)) return 3;
  if (writeHex(argv[1], "python-deflated.zip", deflated)) return 4;
  if (writePayload(argv[1])) return 5;
  return 0;
}
