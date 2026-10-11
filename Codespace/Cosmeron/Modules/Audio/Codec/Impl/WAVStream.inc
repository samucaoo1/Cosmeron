#pragma once

typedef struct AUDIO_INS(TWAVView) {
  const unsigned char *data;
  size_t byteCount;
  uint64_t frameCount;
  uint32_t sampleRate, channels;
  uint16_t format, bits;
} AUDIO_INS(TWAVView);

/* The view borrows the caller's RIFF bytes. No frame buffer is allocated. */
static inline OPSTATUS AUDIO_INS(WAV_Parse)(
    const void *source, size_t size, AUDIO_INS(TWAVView) *outView) {
  static const unsigned char extensibleGuidTail[14] = {
    0, 0, 0, 0, 0x10, 0, 0x80, 0, 0, 0xaa, 0, 0x38, 0x9b, 0x71
  };
  const unsigned char *p = (const unsigned char *)source, *fmt = NULL;
  const unsigned char *samples = NULL;
  size_t offset = 12, riffEnd, fmtSize = 0, dataSize = 0;
  uint32_t rate = 0, byteRate;
  uint16_t format = 0, channels = 0, bits = 0, align = 0;
  bool haveFmt = false, haveData = false;
  AUDIO_INS(TWAVView) view = {0};
  if (!p || !outView || size < 12 ||
      memcmp(p, "RIFF", 4) || memcmp(p + 8, "WAVE", 4))
    return STATUS_CONST(INVALID_ARGUMENT);
  if (AUDIO_INS(WAV_U32)(p + 4) < 4 ||
      (size_t)AUDIO_INS(WAV_U32)(p + 4) > size - 8)
    return STATUS_CONST(INVALID_ARGUMENT);
  riffEnd = (size_t)AUDIO_INS(WAV_U32)(p + 4) + 8;
  while (offset < riffEnd) {
    size_t length, padded;
    const unsigned char *chunk;
    if (riffEnd - offset < 8) return STATUS_CONST(INVALID_ARGUMENT);
    length = (size_t)AUDIO_INS(WAV_U32)(p + offset + 4);
    chunk = p + offset + 8;
    offset += 8;
    if (length > riffEnd - offset) return STATUS_CONST(INVALID_ARGUMENT);
    if (!haveFmt && !memcmp(chunk - 8, "fmt ", 4)) {
      if (length < 16) return STATUS_CONST(INVALID_ARGUMENT);
      fmt = chunk; fmtSize = length; haveFmt = true;
    } else if (!haveData && !memcmp(chunk - 8, "data", 4)) {
      samples = chunk; dataSize = length; haveData = true;
    }
    padded = length + (length & 1U);
    if (padded > riffEnd - offset) {
      if (length != riffEnd - offset) return STATUS_CONST(INVALID_ARGUMENT);
      padded = length;
    }
    offset += padded;
  }
  if (!haveFmt || !haveData) return STATUS_CONST(INVALID_ARGUMENT);
  format = AUDIO_INS(WAV_U16)(fmt);
  channels = AUDIO_INS(WAV_U16)(fmt + 2);
  rate = AUDIO_INS(WAV_U32)(fmt + 4);
  byteRate = AUDIO_INS(WAV_U32)(fmt + 8);
  align = AUDIO_INS(WAV_U16)(fmt + 12);
  bits = AUDIO_INS(WAV_U16)(fmt + 14);
  if (format == UINT16_C(0xfffe)) {
    unsigned int validBits;
    if (fmtSize < 40 || AUDIO_INS(WAV_U16)(fmt + 16) < 22 ||
        memcmp(fmt + 26, extensibleGuidTail, sizeof extensibleGuidTail))
      return STATUS_CONST(NOT_SUPPORTED);
    validBits = AUDIO_INS(WAV_U16)(fmt + 18);
    if (validBits != 0 && validBits != bits)
      return STATUS_CONST(NOT_SUPPORTED);
    format = AUDIO_INS(WAV_U16)(fmt + 24);
  }
  if (format == 1) {
    if (bits != 8 && bits != 16 && bits != 24 && bits != 32)
      return STATUS_CONST(NOT_SUPPORTED);
  } else if (format == 3) {
    if (bits != 32 || sizeof(float) != 4 ||
        FLT_RADIX != 2 || FLT_MANT_DIG != 24 || FLT_MAX_EXP != 128)
      return STATUS_CONST(NOT_SUPPORTED);
  } else return STATUS_CONST(NOT_SUPPORTED);
  if (!AUDIO_INS(ValidFormat)(rate, channels) ||
      align != (uint32_t)channels * (bits / 8) ||
      (uint64_t)rate * align != byteRate || !align ||
      dataSize % align != 0)
    return STATUS_CONST(INVALID_ARGUMENT);
  view.data = samples;
  view.byteCount = dataSize;
  view.frameCount = dataSize / align;
  view.channels = channels;
  view.sampleRate = rate;
  view.format = format;
  view.bits = bits;
  *outView = view;
  return STATUS_CONST(SUCCESS);
}

/* Decode a bounded, interleaved PCM slice without buffering the whole file. */
static inline OPSTATUS AUDIO_INS(WAV_ReadRange)(
    const AUDIO_INS(TWAVView) *view, uint64_t first,
    float *outSamples, size_t frameCapacity, size_t *outFrames) {
  const unsigned char *source;
  size_t frames, i, count, stride;
  if (!view || !outFrames || first > view->frameCount ||
      (frameCapacity && !outSamples) ||
      (frameCapacity > SIZE_MAX / view->channels / sizeof(float)))
    return STATUS_CONST(INVALID_ARGUMENT);
  frames = (uint64_t)frameCapacity > view->frameCount - first ?
      (size_t)(view->frameCount - first) : frameCapacity;
  count = frames * view->channels;
  stride = view->bits / 8;
  source = view->data + (size_t)first * view->channels * stride;
  for (i = 0; i < count; ++i) {
    float sample;
    const unsigned char *p = source + i * stride;
    if (view->format == 3) {
      uint32_t raw = AUDIO_INS(WAV_U32)(p);
      memcpy(&sample, &raw, sizeof sample);
      if (!AUDIO_INS(Finite)(sample)) {
        *outFrames = i / view->channels;
        return STATUS_CONST(INVALID_SEQUENCE);
      }
    } else if (view->bits == 8) {
      sample = ((float)p[0] - 128.0f) / 128.0f;
    } else if (view->bits == 16) {
      unsigned int raw = AUDIO_INS(WAV_U16)(p);
      int32_t signedValue = raw >= 32768U ? (int32_t)raw - 65536 : (int32_t)raw;
      sample = (float)signedValue / 32768.0f;
    } else if (view->bits == 24) {
      uint32_t raw = (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
                     ((uint32_t)p[2] << 16);
      int32_t signedValue = (raw & UINT32_C(0x800000)) ?
          (int32_t)raw - 16777216 : (int32_t)raw;
      sample = (float)signedValue / 8388608.0f;
    } else {
      uint32_t raw = AUDIO_INS(WAV_U32)(p);
      int64_t signedValue = raw >= UINT32_C(0x80000000) ?
          (int64_t)raw - INT64_C(4294967296) : (int64_t)raw;
      sample = (float)signedValue / 2147483648.0f;
    }
    outSamples[i] = sample;
  }
  *outFrames = frames;
  return STATUS_CONST(SUCCESS);
}
