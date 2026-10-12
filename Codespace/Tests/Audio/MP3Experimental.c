#include "../../Cosmeron/Modules/Audio/Codec/MP3Experimental.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void syntheticTests(void) {
  AUDIO_CODEC_TYPE(TMP3Experimental) decoder = {0}, snapshot;
  AUDIO_CODEC_TYPE(TMP3FrameInfo) info = {0};
  unsigned char frame[417] = {0};
  float pcm[2304];
  size_t frames = 0, i;
  OPSTATUS status;
  frame[0] = 0xff; frame[1] = 0xfb; frame[2] = 0x90; frame[3] = 0xc4;
  for (i = 0; i < 2304; ++i) pcm[i] = 987.f;
  status = AUDIO_CODEC_FUNC(MP3_ExperimentFrame)(
      &decoder, frame, 416, pcm, 1152, &frames, &info);
  assert(status == STATUS_CONST(INCOMPLETE_SEQUENCE) &&
      frames == 0 && pcm[0] == 987 && decoder.sampleRate == 0);
  status = AUDIO_CODEC_FUNC(MP3_ExperimentFrame)(
      &decoder, frame, sizeof frame, pcm, 1152, &frames, &info);
  assert(status == STATUS_CONST(SUCCESS) && frames == 1152 &&
      info.channels == 1 && info.sampleRate == 44100);
  for (i = 0; i < frames; ++i) assert(pcm[i] == 0);
  snapshot = decoder;
  pcm[0] = 889.f;
  assert(AUDIO_CODEC_FUNC(MP3_ExperimentFrame)(
      &decoder, frame, sizeof frame, pcm, 1151, &frames, &info) ==
      STATUS_CONST(OUT_OF_RANGE));
  assert(pcm[0] == 889 && !memcmp(&snapshot, &decoder, sizeof decoder));

  frame[3] = 0x04; /* stereo requires independent stream state */
  assert(AUDIO_CODEC_FUNC(MP3_ExperimentFrame)(
      &decoder, frame, sizeof frame, pcm, 1152, &frames, &info) ==
      STATUS_CONST(INVALID_SEQUENCE));
  memset(&decoder, 0, sizeof decoder);
  assert(AUDIO_CODEC_FUNC(MP3_ExperimentFrame)(
      &decoder, frame, sizeof frame, pcm, 1152, &frames, &info) ==
      STATUS_CONST(SUCCESS));
  assert(info.channels == 2 && frames == 1152);
  for (i = 0; i < frames * 2; ++i) assert(pcm[i] == 0);

  frame[3] = 0x54; /* MPEG-1 joint intensity: synthetic zero-energy bands */
  pcm[0] = 789.f;
  assert(AUDIO_CODEC_FUNC(MP3_ExperimentFrame)(
      &decoder, frame, sizeof frame, pcm, 1152, &frames, &info) ==
      STATUS_CONST(SUCCESS));
  assert(info.intensityStereo && frames == 1152);
  for (i = 0; i < frames * 2; ++i) assert(pcm[i] == 0);
  pcm[0] = 789.f;

  /* MPEG-2.5, 8-kHz zero-energy frame on a fresh logical stream.
   * Reusing the 44.1-kHz state is intentionally invalid, but a clean
   * decoder should produce exactly 576 zero PCM frames. */
  frame[1] = 0xe3;
  frame[2] = 0x48;
  frame[3] = 0xc4;
  snapshot = decoder;
  pcm[0] = 789.f;
  assert(AUDIO_CODEC_FUNC(MP3_ExperimentFrame)(
      &decoder, frame, sizeof frame, pcm, 576, &frames, &info) ==
      STATUS_CONST(INVALID_SEQUENCE));
  assert(pcm[0] == 789.f && !memcmp(&snapshot,&decoder,sizeof decoder));
  {
    AUDIO_CODEC_TYPE(TMP3Experimental) lowRate = {0};
    assert(AUDIO_CODEC_FUNC(MP3_ExperimentFrame)(
        &lowRate, frame, sizeof frame, pcm, 576, &frames, &info) ==
        STATUS_CONST(SUCCESS));
    assert(info.sampleRate == 8000 && frames == 576);
    for (i = 0; i < frames; ++i) assert(pcm[i] == 0.f);
  }
  frame[1] = 0xfb;
  frame[2] = 0x90;

  /* Missing prior main_data_begin does not publish PCM, but contributes
   * current compressed payload bytes to a future frame's reservoir. */
  memset(&decoder, 0, sizeof decoder);
  frame[3] = 0xc4;
  frame[5] = 0x80; /* nine-bit main_data_begin = 1 */
  frames = 0;
  assert(AUDIO_CODEC_FUNC(MP3_ExperimentFrame)(
      &decoder, frame, sizeof frame, pcm, 1152, &frames, &info) ==
      STATUS_CONST(INCOMPLETE_SEQUENCE));
  assert(decoder.reservoir.historySize != 0 && decoder.sampleRate == 0 &&
      pcm[0] == 789 && frames == 0);
  frame[5] = 0;
  assert(AUDIO_CODEC_FUNC(MP3_ExperimentFrame)(
      &decoder, frame, sizeof frame, pcm, 1152, &frames, &info) ==
      STATUS_CONST(SUCCESS));
}

static void realFrames(void) {
  unsigned char bytes[65536];
  AUDIO_CODEC_TYPE(TMP3Experimental) decoder = {0};
  AUDIO_CODEC_TYPE(TMP3FrameInfo) info;
  FILE *file = fopen("Fixtures/Tone.mp3", "rb");
  float pcm[2304];
  size_t length, offset = 0, frames, decoded = 0, total = 0;
  size_t unsupported = 0, preRoll = 0;
  double energy = 0;
  assert(file);
  length = fread(bytes, 1, sizeof bytes, file);
  assert(!fclose(file) && length > 1000);
  assert(AUDIO_CODEC_FUNC(MP3_FindFrame)(bytes, length,
      0, &offset, &info) == STATUS_CONST(SUCCESS));
  while (length - offset >= 4 && decoded < 25) {
    OPSTATUS status;
    size_t i;
    assert(AUDIO_CODEC_FUNC(MP3_ParseHeader)(
        bytes+offset,length-offset,&info) == STATUS_CONST(SUCCESS));
    if (info.frameBytes > length-offset) break;
    frames = 0;
    status = AUDIO_CODEC_FUNC(MP3_ExperimentFrame)(
        &decoder, bytes+offset, info.frameBytes, pcm, 1152, &frames, &info);
    if (status == STATUS_CONST(SUCCESS)) {
      assert(frames == 1152);
      ++decoded;
      total += frames;
      for (i=0; i<frames*info.channels; ++i)
        energy += pcm[i] < 0 ? -(double)pcm[i] : (double)pcm[i];
    } else if (status == STATUS_CONST(NOT_SUPPORTED)) {
      ++unsupported;
    } else if (status == STATUS_CONST(INCOMPLETE_SEQUENCE)) {
      ++preRoll;
    } else {
      fprintf(stderr, "experimental MP3 frame error %d at %zu\n",
          (int)status, offset);
      assert(0);
    }
    offset += info.frameBytes;
  }
  assert(total > 0 && energy > 0);
  fprintf(stdout,"[mp3 experimental] decoded=%zu preroll=%zu unsupported=%zu "
      "frames=%zu absolute-energy=%.6f (NOT PCM parity)\n",
      decoded,preRoll,unsupported,total,energy);
}

int main(void) {
  syntheticTests();
  realFrames();
  return 0;
}
