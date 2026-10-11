#include "../../Cosmeron/Modules/Audio/CodecMinimal.h"
#include "../../Cosmeron/Modules/Audio/Codec/MP3Experimental.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Diagnostic, not a parity certification. MP3 encoder delay/gapless
 * metadata can shift the two streams by over 1000 samples. Search the
 * entire common valid window and report a least-squares gain as well as
 * residual power. A tonal fixture alone cannot certify sample parity. */
int main(int argc, char **argv) {
  enum { MAX_FRAMES = 48 * 1152, ANALYSIS = 2048, BASE = 1536 };
  unsigned char bytes[65536];
  AUDIO_CODEC_TYPE(TMP3Experimental) native = {0};
  AUDIO_CODEC_TYPE(TMP3FrameInfo) header = {0};
  cosmeron_audio_vendor_ma_decoder reference;
  cosmeron_audio_vendor_ma_decoder_config config;
  cosmeron_audio_vendor_ma_uint64 received = 0;
  const char *path = argc > 1 ? argv[1] : "Fixtures/Tone.mp3";
  AUDIO_CODEC_TYPE(TMP3XingInfo) xing = {0};
  bool hasXing = false;
  FILE *file = fopen(path, "rb");
  float *nativePCM, *referencePCM;
  size_t length, offset = 0, frames = 0, used = 0, decoded = 0;
  size_t skipped = 0, preroll = 0, channels = 0, i;
  double bestRatio = 1e100, bestGain = 0, bestNativeEnergy = 0;
  int bestShift = 0, shift;
  assert(file);
  length = fread(bytes, 1, sizeof bytes, file);
  assert(!fclose(file) && length > 1000);
  nativePCM = (float *)malloc(MAX_FRAMES * 2U * sizeof(float));
  referencePCM = (float *)malloc(MAX_FRAMES * 2U * sizeof(float));
  assert(nativePCM && referencePCM);
  assert(AUDIO_CODEC_FUNC(MP3_FindFrame)(bytes, length, 0,
      &offset, &header) == STATUS_CONST(SUCCESS));
  hasXing = AUDIO_CODEC_FUNC(MP3_ParseXing)(
      bytes + offset, length - offset, &xing) == STATUS_CONST(SUCCESS);
  while (length - offset >= 4 && used + 1152 <= MAX_FRAMES) {
    OPSTATUS result;
    size_t produced = 0;
    assert(AUDIO_CODEC_FUNC(MP3_ParseHeader)(bytes + offset,
        length - offset, &header) == STATUS_CONST(SUCCESS));
    if (header.frameBytes > length - offset) break;
    result = AUDIO_CODEC_FUNC(MP3_ExperimentFrame)(&native,
        bytes + offset, header.frameBytes,
        nativePCM + used * header.channels, MAX_FRAMES - used,
        &produced, &header);
    if (result == STATUS_CONST(SUCCESS)) {
      assert(produced == 1152);
      channels = header.channels;
      used += produced;
      ++decoded;
    } else if (result == STATUS_CONST(INCOMPLETE_SEQUENCE)) ++preroll;
    else if (result == STATUS_CONST(NOT_SUPPORTED)) ++skipped;
    else {
      fprintf(stderr, "native MP3 stage failed at byte %zu: %d\n",
          offset, (int)result);
      assert(0);
    }
    offset += header.frameBytes;
  }
  assert(decoded >= 2 && used >= 2304 && channels > 0);
  config = cosmeron_audio_vendor_ma_decoder_config_init(
      cosmeron_audio_vendor_ma_format_f32, 0, 0);
  assert(cosmeron_audio_vendor_ma_decoder_init_memory(
      bytes, length, &config, &reference) ==
      cosmeron_audio_vendor_MA_SUCCESS);
  assert(reference.outputChannels == channels);
  {
    const cosmeron_audio_vendor_ma_result result =
        cosmeron_audio_vendor_ma_decoder_read_pcm_frames(
          &reference, referencePCM, MAX_FRAMES, &received);
    assert(result == cosmeron_audio_vendor_MA_SUCCESS ||
           result == cosmeron_audio_vendor_MA_AT_END);
  }
  frames = (size_t)received;
  if (used >= BASE + ANALYSIS && frames >= ANALYSIS) {
    const int minimum = -(int)BASE;
    const int maximum = (int)frames - (BASE + ANALYSIS);
    for (shift = minimum; shift <= maximum; shift += 4) {
      double xx = 0, xy = 0, yy = 0, residual, gain;
      for (i = 0; i < ANALYSIS * channels; ++i) {
        const size_t sample = i / channels, ch = i % channels;
        const size_t a = (BASE + sample) * channels + ch;
        const size_t b = (size_t)((int)(BASE + sample) + shift) *
                         channels + ch;
        const double x = nativePCM[a], y = referencePCM[b];
        xx += x * x;
        xy += x * y;
        yy += y * y;
      }
      if (xx < 1e-12 || yy < 1e-12) continue;
      gain = xy / xx;
      residual = 1.0 - xy * xy / (xx * yy);
      if (residual < bestRatio) {
        bestRatio = residual;
        bestGain = gain;
        bestNativeEnergy = xx / (ANALYSIS * channels);
        bestShift = shift;
      }
    }
    for (shift = bestShift - 4; shift <= bestShift + 4; ++shift) {
      double xx = 0, xy = 0, yy = 0, residual, gain;
      if (shift < minimum || shift > maximum) continue;
      for (i = 0; i < ANALYSIS * channels; ++i) {
        const size_t sample = i / channels, ch = i % channels;
        const size_t a = (BASE + sample) * channels + ch;
        const size_t b = (size_t)((int)(BASE + sample) + shift) *
                         channels + ch;
        const double x = nativePCM[a], y = referencePCM[b];
        xx += x * x;
        xy += x * y;
        yy += y * y;
      }
      if (xx < 1e-12 || yy < 1e-12) continue;
      gain = xy / xx;
      residual = 1.0 - xy * xy / (xx * yy);
      if (residual < bestRatio) {
        bestRatio = residual;
        bestGain = gain;
        bestNativeEnergy = xx / (ANALYSIS * channels);
        bestShift = shift;
      }
    }
  }
  printf("[mp3 experimental parity] path=%s native=%zu reference=%zu "
         "decoded=%zu preroll=%zu unsupported=%zu "
         "best-shift=%d best-gain=%.8g normalized-residual=%.8g "
         "native-mean-square=%.8g xing=%u encoded-frames=%u "
         "encoder-delay=%u end-padding=%u (DIAGNOSTIC ONLY)\\n",
         path, used, frames, decoded, preroll, skipped,
         bestShift, bestGain, bestRatio, bestNativeEnergy,
         (unsigned int)hasXing, (unsigned int)xing.encodedFrames,
         (unsigned int)xing.encoderDelay, (unsigned int)xing.endPadding);
  assert(bestRatio >= -0.0000001 && bestRatio <= 1.0000001);
  cosmeron_audio_vendor_ma_decoder_uninit(&reference);
  free(nativePCM);
  free(referencePCM);
  return 0;
}
