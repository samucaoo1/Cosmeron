#include "../../Cosmeron/Modules/Audio/Codec/MP3.h"
#include <assert.h>
#include <string.h>

static bool closeTo(float a, float b) {
  float delta = a-b;
  return delta < 0.00005f && delta > -0.00005f;
}
int main(void) {
  const unsigned char jointMS[4] = {0xff,0xfb,0x90,0x64};
  const unsigned char jointIntensity[4] = {0xff,0xfb,0x90,0x54};
  const unsigned char jointBoth[4] = {0xff,0xfb,0x90,0x74};
  const unsigned char normalStereo[4] = {0xff,0xfb,0x90,0x04};
  AUDIO_CODEC_TYPE(TMP3FrameInfo) hdr;
  float left[576] = {0}, right[576] = {0}, outL[576], outR[576];
  unsigned int position;
  size_t i;
  assert(AUDIO_CODEC_FUNC(MP3_ParseHeader)(
      jointMS,4,&hdr) == STATUS_CONST(SUCCESS));
  assert(hdr.channelMode == 1 && hdr.modeExtension == 2 &&
      hdr.msStereo && !hdr.intensityStereo);
  assert(AUDIO_CODEC_FUNC(MP3_ParseHeader)(
      jointIntensity,4,&hdr) == STATUS_CONST(SUCCESS));
  assert(hdr.channelMode == 1 && hdr.modeExtension == 1 &&
      !hdr.msStereo && hdr.intensityStereo);
  assert(AUDIO_CODEC_FUNC(MP3_ParseHeader)(
      jointBoth,4,&hdr) == STATUS_CONST(SUCCESS));
  assert(hdr.modeExtension == 3 &&
      hdr.msStereo && hdr.intensityStereo);
  assert(AUDIO_CODEC_FUNC(MP3_ParseHeader)(
      normalStereo,4,&hdr) == STATUS_CONST(SUCCESS));
  assert(hdr.channelMode == 0 && hdr.modeExtension == 0 &&
      !hdr.msStereo && !hdr.intensityStereo);

  for (i = 0; i < 576; ++i) left[i] = (float)(i % 10);
  for (position = 0; position < 7; ++position) {
    assert(AUDIO_CODEC_FUNC(MP3_IntensityBand)(
        left, right, 16, 64, position, outL, outR) == STATUS_CONST(SUCCESS));
    for (i = 0; i < 576; ++i) {
      if (i >= 16 && i < 64) {
        assert(closeTo(outL[i]+outR[i], left[i]));
        if (position == 0) assert(outL[i] == 0 && outR[i] == left[i]);
        if (position == 3)
          assert(closeTo(outL[i], left[i]*0.5f));
        if (position == 6) assert(outL[i] == left[i] && outR[i] == 0);
      } else {
        assert(outL[i] == left[i] && outR[i] == right[i]);
      }
    }
  }
  right[17] = 8.f;
  outL[0] = 932.f;
  assert(AUDIO_CODEC_FUNC(MP3_IntensityBand)(
      left, right, 16, 64, 4, outL, outR) ==
      STATUS_CONST(INVALID_SEQUENCE));
  assert(outL[0] == 932.f);
  assert(AUDIO_CODEC_FUNC(MP3_IntensityBand)(
      left, right, 16, 64, 7, outL, outR) == STATUS_CONST(SUCCESS));
  assert(outR[17] == 8.f);
  right[17] = 0;
  assert(AUDIO_CODEC_FUNC(MP3_IntensityBand)(
      left, right, 0, 576, 1, left, right) == STATUS_CONST(SUCCESS));
  assert(closeTo(left[19] + right[19], 9.f));
  assert(AUDIO_CODEC_FUNC(MP3_IntensityBand)(
      left, right, 0, 576, 8, outL, outR) == STATUS_CONST(INVALID_ARGUMENT));
  assert(AUDIO_CODEC_FUNC(MP3_IntensityBand)(
      left, right, 0, 576, 3, outL, outL) == STATUS_CONST(INVALID_ARGUMENT));
  assert(AUDIO_CODEC_FUNC(MP3_IntensityBand)(
      left, right, 200, 199, 3, outL, outR) == STATUS_CONST(INVALID_ARGUMENT));
  return 0;
}
