#include "../../Cosmeron/Modules/Compression/Compression.h"
#include <assert.h>
#include <string.h>
#include "../../Cosmeron/Modules/Compression/Codec/Impl/Inflate.impl"

int main(void) {
  static const unsigned char stored[] = {1, 3, 0, 252, 255, 'a', 'b', 'c'};
  static const unsigned char fixed[] = {0x4b, 0x4c, 0x4a, 0x06, 0x00};
  unsigned char output[64] = {0};
  size_t used = 0, written = 0;
  assert(Compression_Internal_InflateRaw(stored, sizeof stored, output,
      sizeof output, &used, &written) == STATUS_SUCCESS);
  assert(used == sizeof stored && written == 3 && !memcmp(output, "abc", 3));
  assert(Compression_Internal_InflateRaw(fixed, sizeof fixed, output,
      sizeof output, &used, &written) == STATUS_SUCCESS);
  assert(used == sizeof fixed && written == 3 && !memcmp(output, "abc", 3));
  assert(Compression_Internal_InflateRaw(fixed, 2, output, sizeof output,
      &used, &written) == STATUS_INCOMPLETE_SEQUENCE);
  assert(Compression_Internal_InflateRaw(stored, sizeof stored, output, 2,
      &used, &written) == STATUS_INSUFFICIENT_SPACE);

  {
    static const unsigned char dynamic[] = {
      0x05,0xc1,0x87,0x01,0xc0,0x20,0x0c,0x03,0xb0,0x5b,0x19,0x01,
      0x5c,0x82,0xc3,0x0a,0xf7,0x57,0xf2,0x1c,0x9e,0x24,0xe2,0x96,
      0xd7,0xe8,0xe6,0x16,0x2f,0x9b,0x9c,0xe1,0xeb,0x0d,0x63,0xf5,
      0xb6,0x9c,0x79,0x9b,0x44,0x0d,0xae,0x0a,0xde,0x86,0xd1,0x38,
      0x3e,0xd3,0xf4,0xf0,0xc1,0x5d,0x91,0xae,0x0c,0x34,0x48,0xd0,
      0x4d,0xb1,0x88,0xed,0xfd,0x74,0xe5,0x9b,0x7d,0x12,0x6b,0xd3,
      0x83,0x12,0x62,0x5f,0x8d,0x0f,0xa9,0xd4,0x23,0x35,0x75,0xc1,
      0xf4,0x13,0x70,0x82,0x74,0xac,0xd9,0x4b,0xa9,0x91,0xf9,0x07
    };
    static const char expected[] =
      "udavecnitfvhnuouobtnhesmuqvmonguhqundroeblaullinthimhnmjolcvijiuulictemihiealrneobiruksklnvpkpniqrnualnieojgbvicfgsegckeipusaisaekiqpkffgbnd";
    unsigned char larger[sizeof expected] = {0};
    assert(Compression_Internal_InflateRaw(dynamic, sizeof dynamic, larger,
        sizeof larger, &used, &written) == STATUS_SUCCESS);
    assert(used == sizeof dynamic && written == sizeof expected - 1 &&
        !memcmp(larger, expected, written));
  }
  return 0;
}
