#include "../../Cosmeron/Modules/Audio/CodecMinimal.h"
#include "../../Cosmeron/Modules/Audio/Codec/MP3Experimental.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Gain-compensated normalized residual between two corresponding windows.
 * This is diagnostic: the output is 0 for equal signals up to gain,
 * and 1 for orthogonal signals. It does not prove MP3 fidelity. */
static double compare_at(const float *native, const float *reference,
    size_t anchor, size_t count, size_t channels, int shift,
    double *outGain, double *outMeanSquare) {
  size_t i;
  double xx = 0, xy = 0, yy = 0;
  for (i = 0; i < count * channels; ++i) {
    const size_t sample = i / channels, channel = i % channels;
    const size_t a = (anchor + sample) * channels + channel;
    const size_t b = (size_t)((int)(anchor + sample) + shift) *
                     channels + channel;
    const double x = native[a], y = reference[b];
    xx += x*x;
    xy += x*y;
    yy += y*y;
  }
  if (xx < 1e-14 || yy < 1e-14) return 1e100;
  *outGain = xy/xx;
  *outMeanSquare = xx/(count*channels);
  return 1.0 - xy*xy/(xx*yy);
}

/* Diagnostic, not a parity certification. MP3 encoder delay/gapless
 * metadata can shift the two streams by over 1000 samples. Search the
 * entire common valid window and report a least-squares gain as well as
 * residual power. A tonal fixture alone cannot certify sample parity. */
int main(int argc, char **argv) {
  enum { MAX_FRAMES = 48 * 1152, ANALYSIS = 2048 };
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
  size_t skipped = 0, preroll = 0, channels = 0, msFrames = 0, intensityFrames = 0;
  double bestRatio = 1e100, bestGain = 0, bestNativeEnergy = 0;
  double metadataRatio = 1e100, metadataGain = 0, metadataEnergy = 0;
  size_t trimmedStart = 0, expectedGaplessFrames = 0, anchor = 1536;
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
    if (header.frameBytes > length - offset ||
        used + header.samplesPerFrame > MAX_FRAMES) break;
    if (header.msStereo) ++msFrames;
    if (header.intensityStereo) ++intensityFrames;
    result = AUDIO_CODEC_FUNC(MP3_ExperimentFrame)(&native,
        bytes + offset, header.frameBytes,
        nativePCM + used * header.channels, MAX_FRAMES - used,
        &produced, &header);
    if (result == STATUS_CONST(SUCCESS)) {
      assert(produced == header.samplesPerFrame);
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
  /* XING frame count excludes the Info frame for the LAME fixtures.
   * Keep the assumption explicit: it is checked against actual counts
   * rather than silently treating all MP3 encoders as identical. */
  if (hasXing && xing.hasGapless && xing.encodedFrames > 0 &&
      decoded >= xing.encodedFrames &&
      decoded - xing.encodedFrames <= 2) {
    uint64_t rawFirst = 0, rawCount = 0;
    if (AUDIO_CODEC_FUNC(MP3_GaplessSpan)(&xing,
        header.samplesPerFrame,
        (uint32_t)(decoded - xing.encodedFrames),
        &rawFirst, &rawCount) == STATUS_CONST(SUCCESS) &&
        rawFirst < used && rawCount <= SIZE_MAX) {
      trimmedStart = (size_t)rawFirst;
      expectedGaplessFrames = (size_t)rawCount;
      anchor = trimmedStart + 384;
    }
  }
  if (used >= anchor + ANALYSIS && frames >= ANALYSIS) {
    const int minimum = -(int)anchor;
    const int maximum = (int)frames - (int)(anchor + ANALYSIS);
    for (shift = minimum; shift <= maximum; shift += 4) {
      double gain = 0, square = 0;
      double residual = compare_at(nativePCM, referencePCM,
          anchor, ANALYSIS, channels, shift, &gain, &square);
      if (residual < bestRatio) {
        bestRatio = residual;
        bestGain = gain;
        bestNativeEnergy = square;
        bestShift = shift;
      }
    }
    for (shift = bestShift - 4; shift <= bestShift + 4; ++shift) {
      double gain = 0, square = 0, residual;
      if (shift < minimum || shift > maximum) continue;
      residual = compare_at(nativePCM, referencePCM,
          anchor, ANALYSIS, channels, shift, &gain, &square);
      if (residual < bestRatio) {
        bestRatio = residual;
        bestGain = gain;
        bestNativeEnergy = square;
        bestShift = shift;
      }
    }
    if (trimmedStart && -(int)trimmedStart >= minimum &&
        -(int)trimmedStart <= maximum)
      metadataRatio = compare_at(nativePCM, referencePCM,
          anchor, ANALYSIS, channels, -(int)trimmedStart,
          &metadataGain, &metadataEnergy);
  }
  printf("[mp3 experimental parity] path=%s native=%zu reference=%zu "
         "decoded=%zu preroll=%zu unsupported=%zu "
         "best-shift=%d best-gain=%.8g normalized-residual=%.8g "
         "metadata-leading=%zu metadata-play-frames=%zu "
         "metadata-residual=%.8g metadata-gain=%.8g "
         "native-mean-square=%.8g ms-frames=%zu intensity-frames=%zu "
         "xing=%u encoded-frames=%u "
         "encoder-delay=%u end-padding=%u (DIAGNOSTIC ONLY)\n",
         path, used, frames, decoded, preroll, skipped,
         bestShift, bestGain, bestRatio, trimmedStart,
         expectedGaplessFrames, metadataRatio, metadataGain,
         bestNativeEnergy, msFrames, intensityFrames,
         (unsigned int)hasXing, (unsigned int)xing.encodedFrames,
         (unsigned int)xing.encoderDelay, (unsigned int)xing.endPadding);
  /* A constant encoder delay should imply the same lag in every section.
   * Diverging best-shifts indicate drift or a reconstruction problem,
   * rather than an ordinary one-off leading-sample trim. */
  if (trimmedStart && expectedGaplessFrames >= 14000) {
    unsigned int section;
    for (section = 0; section < 3; ++section) {
      const size_t blockAnchor = trimmedStart + 384U + section * 5000U;
      const size_t blockLength = 1024U;
      int lower, upper, lag, winner = 0;
      double best = 1e100, gainBest = 0, meanSquare = 0;
      if (blockAnchor + blockLength > used ||
          frames < blockLength) continue;
      lower = -(int)blockAnchor;
      upper = (int)frames - (int)(blockAnchor + blockLength);
      for (lag = lower; lag <= upper; lag += 8) {
        double gain = 0, energy = 0;
        double ratio = compare_at(nativePCM, referencePCM, blockAnchor,
            blockLength, channels, lag, &gain, &energy);
        if (ratio < best) {
          best = ratio;
          winner = lag;
          gainBest = gain;
          meanSquare = energy;
        }
      }
      for (lag = winner - 8; lag <= winner + 8; ++lag) {
        double gain = 0, energy = 0, ratio;
        if (lag < lower || lag > upper) continue;
        ratio = compare_at(nativePCM, referencePCM, blockAnchor,
            blockLength, channels, lag, &gain, &energy);
        if (ratio < best) {
          best = ratio;
          winner = lag;
          gainBest = gain;
          meanSquare = energy;
        }
      }
      printf("[mp3 parity segment] section=%u anchor=%zu shift=%d "
          "normalized-residual=%.8g gain=%.8g native-energy=%.8g\n",
          section, blockAnchor, winner, best, gainBest, meanSquare);
    }
  }
  fflush(stdout); /* preserve diagnostics if a strict parity assertion fails */
  assert(bestRatio >= -0.0000001 && bestRatio <= 1.0000001);
  if (getenv("COSMERON_MP3_PARITY_STRICT") != NULL) {
    assert(hasXing && trimmedStart && expectedGaplessFrames == frames);
    assert(metadataRatio >= -0.0000001 && metadataRatio < 0.00001);
    assert(metadataGain > 0.999 && metadataGain < 1.001);
    if (getenv("COSMERON_MP3_REQUIRE_MS") != NULL)
      assert(msFrames > 0);

  }
  cosmeron_audio_vendor_ma_decoder_uninit(&reference);
  free(nativePCM);
  free(referencePCM);
  return 0;
}
