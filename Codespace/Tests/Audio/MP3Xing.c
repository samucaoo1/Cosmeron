#include "../../Cosmeron/Modules/Audio/Codec/MP3.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  unsigned char frame[417] = {0};
  AUDIO_CODEC_TYPE(TMP3XingInfo) info = {0};
  const size_t xing = 36, lame = 52;
  frame[0] = 0xff;
  frame[1] = 0xfb;
  frame[2] = 0x90;
  frame[3] = 0x04; /* MPEG1 stereo, 128 kbit/s, 44.1 kHz */
  memcpy(frame + xing, "Xing", 4);
  frame[xing + 7] = 3; /* frame and total-byte count */
  frame[xing + 11] = 6;
  frame[xing + 15] = 0x42;
  memcpy(frame + lame, "LAME", 4);
  frame[lame + 21] = 0x24; /* encoder delay = 576 */
  frame[lame + 22] = 0x00;
  frame[lame + 23] = 0x64; /* padding = 100 */
  assert(AUDIO_CODEC_FUNC(MP3_ParseXing)(frame, sizeof frame, &info) ==
      STATUS_CONST(SUCCESS));
  assert(info.variableBitrate && info.flags == 3 &&
      info.encodedFrames == 6 && info.encodedBytes == 0x42 &&
      info.hasGapless && info.encoderDelay == 576 &&
      info.endPadding == 100 && !info.hasTOC);
  {
    uint64_t first = 0, count = 0;
    /* Insufficient padding to cover 529 decoder priming requires
     * a decoder with tail flushing: reject fixed-buffer indexing. */
    assert(AUDIO_CODEC_FUNC(MP3_GaplessSpan)(&info, 1152, 1,
        &first, &count) == STATUS_CONST(NOT_SUPPORTED));
    frame[lame + 22] = 0x02; /* end padding 0x2d0 = 720 */
    frame[lame + 23] = 0xd0;
    assert(AUDIO_CODEC_FUNC(MP3_ParseXing)(frame, sizeof frame, &info) ==
        STATUS_CONST(SUCCESS));
    assert(AUDIO_CODEC_FUNC(MP3_GaplessSpan)(&info, 1152, 1,
        &first, &count) == STATUS_CONST(SUCCESS));
    assert(first == 2257 && count == 5616);
  }
  frame[xing + 7] = 1;
  memset(frame + xing + 12, 0, 120);
  assert(AUDIO_CODEC_FUNC(MP3_ParseXing)(frame, sizeof frame, &info) ==
      STATUS_CONST(SUCCESS));
  assert(!info.hasGapless && info.encodedFrames == 6 &&
      info.encodedBytes == 0);
  assert(AUDIO_CODEC_FUNC(MP3_ParseXing)(frame, 416, &info) ==
      STATUS_CONST(INCOMPLETE_SEQUENCE));
  frame[xing + 7] = 0x80;
  assert(AUDIO_CODEC_FUNC(MP3_ParseXing)(frame, sizeof frame, &info) ==
      STATUS_CONST(INVALID_SEQUENCE));
  memcpy(frame + xing, "JUNK", 4);
  assert(AUDIO_CODEC_FUNC(MP3_ParseXing)(frame, sizeof frame, &info) ==
      STATUS_CONST(NOT_FOUND));

  {
    FILE *file = fopen("Fixtures/Tone.mp3", "rb");
    unsigned char data[65536];
    size_t n, offset;
    AUDIO_CODEC_TYPE(TMP3FrameInfo) header;
    OPSTATUS status;
    assert(file != NULL);
    n = fread(data, 1, sizeof data, file);
    assert(!fclose(file) && n > 1000);
    assert(AUDIO_CODEC_FUNC(MP3_FindFrame)(
        data, n, 0, &offset, &header) == STATUS_CONST(SUCCESS));
    status = AUDIO_CODEC_FUNC(MP3_ParseXing)(data + offset,
        n - offset, &info);
    assert(status == STATUS_CONST(SUCCESS) ||
           status == STATUS_CONST(NOT_FOUND));
    if (status == STATUS_CONST(SUCCESS))
      printf("[mp3 Xing] frames=%u delay=%u padding=%u has-gapless=%u\n",
          (unsigned int)info.encodedFrames, (unsigned int)info.encoderDelay,
          (unsigned int)info.endPadding, (unsigned int)info.hasGapless);
  }
  return 0;
}
