#pragma once
/*
 * EXPERIMENTAL: MPEG Layer III frame -> interleaved f32 PCM.
 * This is an opt-in, header-only development API, not a replacement for
 * Audio_Codec_OpenFile/OpenMemory/OpenReader.
 * MPEG-2/2.5 intensity stereo, MS+intensity, long/short/mixed frame
 * modes have independent authored-bitstream PCM parity regression tests.
 * Full MP3 streaming, seeking, gapless variants and broader encoder
 * interoperability are not yet certified.
 */
#include "MP3.h"

/* One zero-initialized instance per logical MP3 stream. Do not use it for
 * concurrent frames or share it between unrelated files. */
typedef struct AUDIO_CODEC_TYPE(TMP3Experimental) {
  AUDIO_CODEC_TYPE(TMP3Reservoir) reservoir;
  AUDIO_CODEC_TYPE(TMP3HybridState) hybrid[2];
  AUDIO_CODEC_TYPE(TMP3SynthesisState) synthesis[2];
  uint32_t sampleRate;
  uint8_t channels;
} AUDIO_CODEC_TYPE(TMP3Experimental);

#define AUDIO_CODEC_MP3_EXPERIMENT_FRAME_PROTOTYPE                            \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_ExperimentFrame)(              \
      AUDIO_CODEC_TYPE(TMP3Experimental) *state,                              \
      const void *frame, size_t size,                                        \
      float *outInterleaved, size_t frameCapacity,                            \
      size_t *outFrames, AUDIO_CODEC_TYPE(TMP3FrameInfo) *outInfo)

AUDIO_CODEC_MP3_EXPERIMENT_FRAME_PROTOTYPE;

#include "Impl/MP3Experiment.inc"
