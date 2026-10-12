#include "../../Cosmeron/Modules/Audio/Codec/Impl/FLACFrame.inc"
#include <assert.h>
#include <stdint.h>

int main(void) {
  int64_t result = 777;
  assert(AUDIO_INS(FLAC_AddI64)(INT64_MAX, 0, &result));
  assert(result == INT64_MAX);
  assert(!AUDIO_INS(FLAC_AddI64)(INT64_MAX, 1, &result));
  assert(!AUDIO_INS(FLAC_AddI64)(INT64_MIN, -1, &result));
  assert(AUDIO_INS(FLAC_AddI64)(INT64_MIN, INT64_MAX, &result));
  assert(result == -1);

  assert(AUDIO_INS(FLAC_SubI64)(INT64_MIN, 0, &result));
  assert(result == INT64_MIN);
  assert(!AUDIO_INS(FLAC_SubI64)(INT64_MIN, 1, &result));
  assert(!AUDIO_INS(FLAC_SubI64)(INT64_MAX, -1, &result));
  assert(AUDIO_INS(FLAC_SubI64)(INT64_MIN, INT64_MIN, &result));
  assert(result == 0);

  assert(AUDIO_INS(FLAC_MulI64)(INT64_MIN, 1, &result));
  assert(result == INT64_MIN);
  assert(AUDIO_INS(FLAC_MulI64)(INT64_MIN, 0, &result));
  assert(result == 0);
  assert(!AUDIO_INS(FLAC_MulI64)(INT64_MIN, -1, &result));
  assert(!AUDIO_INS(FLAC_MulI64)(INT64_MAX, 2, &result));
  assert(!AUDIO_INS(FLAC_MulI64)(INT64_MIN, 2, &result));
  assert(!AUDIO_INS(FLAC_MulI64)(INT64_MIN, INT64_MIN, &result));
  assert(AUDIO_INS(FLAC_MulI64)(INT64_MIN / 2, 2, &result));
  assert(result == INT64_MIN);
  assert(AUDIO_INS(FLAC_MulI64)(-3, -7, &result));
  assert(result == 21);
  assert(AUDIO_INS(FLAC_MulI64)(INT64_MAX, 1, &result));
  assert(result == INT64_MAX);

  /* A checked operation never writes to output on overflow. */
  result = 777;
  assert(!AUDIO_INS(FLAC_MulI64)(INT64_MAX, INT64_MAX, &result));
  assert(result == 777);
  return 0;
}
