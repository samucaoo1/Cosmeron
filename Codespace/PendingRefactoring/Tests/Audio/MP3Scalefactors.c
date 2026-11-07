#include "../../Cosmeron/Modules/Audio/Codec/MP3.h"
#include <assert.h>
#include <string.h>

static void putBits(unsigned char *p, size_t *cursor, unsigned int value,
                    unsigned int width) {
  unsigned int i;
  for (i = width; i; --i) {
    size_t at = (*cursor)++;
    if ((value >> (i - 1)) & 1U)
      p[at / 8] |= (unsigned char)(0x80U >> (at & 7U));
  }
}

int main(void) {
  unsigned char data[128] = {0};
  AUDIO_CODEC_TYPE(TMP3SideInfo) side = {0};
  AUDIO_CODEC_TYPE(TMP3Scalefactors) scales = {0}, prior = {0};
  size_t cursor = 0, i, window;
  OPSTATUS status;
  side.granules = 2;
  side.channels = 1;
  side.parts[0][0].scalefacCompress = 15;
  side.parts[0][0].part23Length = 74;
  for (i = 0; i < 21; ++i)
    putBits(data, &cursor, (unsigned int)((i + 1) % (i < 11 ? 16 : 8)),
            i < 11 ? 4 : 3);
  assert(cursor == 74);
  status = AUDIO_CODEC_FUNC(MP3_ParseScalefactors)(
      data, sizeof data, &side, 0, 0, NULL, &scales);
  assert(status == STATUS_CONST(SUCCESS) && scales.part2Bits == 74);
  for (i = 0; i < 21; ++i)
    assert(scales.longBands[i] == ((i + 1) % (i < 11 ? 16 : 8)));
  prior = scales;
  for (i = 0; i < 4; ++i) side.scfsi[0][i] = 1;
  side.parts[1][0].scalefacCompress = 15;
  side.parts[1][0].part23Length = 0;
  memset(&scales, 0, sizeof scales);
  assert(AUDIO_CODEC_FUNC(MP3_ParseScalefactors)(
      data, sizeof data, &side, 1, 0, &prior, &scales) == STATUS_CONST(SUCCESS));
  assert(scales.part2Bits == 0 &&
      !memcmp(scales.longBands, prior.longBands, sizeof scales.longBands));
  assert(AUDIO_CODEC_FUNC(MP3_ParseScalefactors)(
      data, sizeof data, &side, 1, 0, NULL, &scales) ==
      STATUS_CONST(INVALID_ARGUMENT));

  memset(data, 0, sizeof data);
  memset(&side, 0, sizeof side);
  side.granules = 2;
  side.channels = 1;
  side.parts[0][0].windowSwitching = 1;
  side.parts[0][0].blockType = 2;
  side.parts[0][0].scalefacCompress = 15;
  side.parts[0][0].part23Length = 126;
  cursor = 0;
  for (i = 0; i < 12; ++i)
    for (window = 0; window < 3; ++window)
      putBits(data, &cursor, (unsigned int)((i + window + 1) % (i < 6 ? 16 : 8)),
              i < 6 ? 4 : 3);
  assert(cursor == 126);
  assert(AUDIO_CODEC_FUNC(MP3_ParseScalefactors)(
      data, sizeof data, &side, 0, 0, NULL, &scales) == STATUS_CONST(SUCCESS));
  assert(scales.part2Bits == 126);
  for (i = 0; i < 12; ++i)
    for (window = 0; window < 3; ++window)
      assert(scales.shortBands[i][window] ==
             ((i + window + 1) % (i < 6 ? 16 : 8)));
  side.parts[0][0].part23Length = 125;
  assert(AUDIO_CODEC_FUNC(MP3_ParseScalefactors)(
      data, sizeof data, &side, 0, 0, NULL, &scales) ==
      STATUS_CONST(INVALID_SEQUENCE));

  memset(data, 0, sizeof data);
  side.parts[0][0].mixedBlock = 1;
  side.parts[0][0].part23Length = 122;
  cursor = 0;
  for (i = 0; i < 8; ++i)
    putBits(data, &cursor, (unsigned int)(i + 2), 4);
  for (i = 3; i < 12; ++i)
    for (window = 0; window < 3; ++window)
      putBits(data, &cursor, (unsigned int)((i + window) % (i < 6 ? 16 : 8)),
              i < 6 ? 4 : 3);
  assert(cursor == 122);
  assert(AUDIO_CODEC_FUNC(MP3_ParseScalefactors)(
      data, sizeof data, &side, 0, 0, NULL, &scales) == STATUS_CONST(SUCCESS));
  assert(scales.part2Bits == 122 && scales.longBands[7] == 9 &&
      scales.shortBands[2][0] == 0 && scales.shortBands[3][0] == 3);
  side.granules = 1;
  assert(AUDIO_CODEC_FUNC(MP3_ParseScalefactors)(
      data, sizeof data, &side, 0, 0, NULL, &scales) ==
      STATUS_CONST(NOT_SUPPORTED));
  return 0;
}
