#include "../../Cosmeron/Modules/Audio/Codec/MP3.h"
#include <assert.h>
#include <string.h>

static bool closeEnough(float a, float b) {
  float delta = a - b;
  return delta < 0.0001f && delta > -0.0001f;
}
static void reorderTests(void) {
  const uint32_t rates[3] = {44100,48000,32000};
  float source[576], target[576], inplace[576];
  AUDIO_CODEC_TYPE(TMP3Granule) part = {0};
  unsigned int rate, mixed, band, win;
  size_t i, width, src, dst, start, line;
  for (i = 0; i < 576; ++i) source[i] = (float)(i + 1);
  assert(AUDIO_CODEC_FUNC(MP3_ReorderShort)(
      source, &part, 44100, target) == STATUS_CONST(SUCCESS));
  assert(!memcmp(source, target, sizeof source));
  for (rate = 0; rate < 3; ++rate) {
    part.windowSwitching = 1;
    part.blockType = 2;
    for (mixed = 0; mixed < 2; ++mixed) {
      part.mixedBlock = (uint8_t)mixed;
      memcpy(inplace, source, sizeof source);
      assert(AUDIO_CODEC_FUNC(MP3_ReorderShort)(
          source, &part, rates[rate], target) == STATUS_CONST(SUCCESS));
      assert(AUDIO_CODEC_FUNC(MP3_ReorderShort)(
          inplace, &part, rates[rate], inplace) == STATUS_CONST(SUCCESS));
      assert(!memcmp(target, inplace, sizeof target));
      start = mixed ? 36 : 0;
      for (i = 0; i < start; ++i) assert(target[i] == source[i]);
      for (band = mixed ? 3U : 0U; band < 13; ++band) {
        const size_t outputStart =
            3U * AUDIO_INS(MP3_ShortBands)[rate][band];
        width = AUDIO_INS(MP3_ShortBands)[rate][band + 1] -
                AUDIO_INS(MP3_ShortBands)[rate][band];
        for (win = 0; win < 3; ++win) {
          for (line = 0; line < width; ++line) {
            src = start + (size_t)win * width + line;
            dst = outputStart + 3U * line + win;
            assert(target[dst] == source[src]);
          }
        }
        start += 3U * width;
      }
      assert(start == 576);
    }
  }
  part.mixedBlock = 0;
  target[0] = 77.f;
  assert(AUDIO_CODEC_FUNC(MP3_ReorderShort)(
      source, &part, 22050, target) == STATUS_CONST(NOT_SUPPORTED));
  assert(target[0] == 77.f);
  part.blockType = 0;
  assert(AUDIO_CODEC_FUNC(MP3_ReorderShort)(
      source, &part, 44100, target) == STATUS_CONST(INVALID_ARGUMENT));
}
static void stereoTests(void) {
  float m[576] = {0}, s[576] = {0}, l[576], r[576], backup[576];
  size_t i;
  for (i = 0; i < 576; ++i) {
    m[i] = (float)(i % 7);
    s[i] = (float)((int)(i % 5) - 2);
  }
  assert(AUDIO_CODEC_FUNC(MP3_StereoMS)(
      m,s,0,576,l,r) == STATUS_CONST(SUCCESS));
  for (i = 0; i < 576; ++i) {
    assert(closeEnough(l[i],(m[i]+s[i])*0.7071067811865f));
    assert(closeEnough(r[i],(m[i]-s[i])*0.7071067811865f));
    assert(closeEnough(l[i]*l[i]+r[i]*r[i],
        m[i]*m[i]+s[i]*s[i]));
  }
  memcpy(backup,m,sizeof m);
  assert(AUDIO_CODEC_FUNC(MP3_StereoMS)(
      m,s,20,30,m,s) == STATUS_CONST(SUCCESS));
  assert(m[0] == backup[0]);
  assert(closeEnough(m[23], l[23]) && closeEnough(s[23], r[23]));
  assert(AUDIO_CODEC_FUNC(MP3_StereoMS)(
      m,s,30,20,l,r) == STATUS_CONST(INVALID_ARGUMENT));
}
static void aliasTests(void) {
  const float cs0 = 0.8574929257f, ca0 = -0.5144957554f;
  float input[576] = {0}, out[576], original[576], inplace[576];
  AUDIO_CODEC_TYPE(TMP3Granule) part = {0};
  size_t i, sb, band;
  for (i = 0; i < 576; ++i) input[i] = (float)((int)(i % 23) - 10);
  memcpy(original,input,sizeof input);
  assert(AUDIO_CODEC_FUNC(MP3_Antialias)(
      input,&part,out) == STATUS_CONST(SUCCESS));
  memcpy(inplace,input,sizeof input);
  assert(AUDIO_CODEC_FUNC(MP3_Antialias)(
      inplace,&part,inplace) == STATUS_CONST(SUCCESS));
  assert(!memcmp(out,inplace,sizeof out));
  for (sb = 1; sb < 32; ++sb) {
    for (band = 0; band < 8; ++band) {
      size_t x = sb*18 - 1 - band, y = sb*18 + band;
      float before = original[x]*original[x] + original[y]*original[y];
      float after = out[x]*out[x] + out[y]*out[y];
      assert(closeEnough(before,after));
    }
  }
  memset(input,0,sizeof input);
  input[17] = 1.f;
  input[18] = 2.f;
  assert(AUDIO_CODEC_FUNC(MP3_Antialias)(
      input,&part,out) == STATUS_CONST(SUCCESS));
  assert(closeEnough(out[17],cs0 - 2*ca0));
  assert(closeEnough(out[18],2*cs0 + ca0));
  part.windowSwitching = 1;
  part.blockType = 2;
  part.mixedBlock = 0;
  assert(AUDIO_CODEC_FUNC(MP3_Antialias)(
      input,&part,out) == STATUS_CONST(SUCCESS));
  assert(!memcmp(input,out,sizeof input));
  part.mixedBlock = 1;
  assert(AUDIO_CODEC_FUNC(MP3_Antialias)(
      input,&part,out) == STATUS_CONST(SUCCESS));
  assert(closeEnough(out[17],cs0 - 2*ca0));
  assert(closeEnough(out[18],2*cs0 + ca0));
  assert(out[35] == 0 && out[36] == 0);
  part.blockType = 0;
  out[0] = 404.f;
  assert(AUDIO_CODEC_FUNC(MP3_Antialias)(
      input,&part,out) == STATUS_CONST(INVALID_ARGUMENT));
  assert(out[0] == 404.f);
}
int main(void) {
  reorderTests();
  stereoTests();
  aliasTests();
  return 0;
}
