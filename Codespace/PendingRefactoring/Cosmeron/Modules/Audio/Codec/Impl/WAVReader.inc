#pragma once
#include <limits.h>

static inline OPSTATUS AUDIO_INS(WAV_ReadExact)(
    const AUDIO_CODEC_TYPE(TReader) *reader, void *out, size_t bytes) {
  unsigned char *p = (unsigned char *)out;
  while (bytes) {
    size_t count = 0;
    OPSTATUS status = reader->read(reader->userData, p, bytes, &count);
    if (status != STATUS_CONST(SUCCESS)) return status;
    if (!count) return STATUS_CONST(INCOMPLETE_SEQUENCE);
    if (count > bytes) return STATUS_CONST(INVALID_SEQUENCE);
    p += count;
    bytes -= count;
  }
  return STATUS_CONST(SUCCESS);
}

/* A seekable RIFF reader needs only 40 metadata bytes, even for large files. */
static inline OPSTATUS AUDIO_INS(WAV_ParseReader)(
    const AUDIO_CODEC_TYPE(TReader) *reader, AUDIO_INS(TWAVView) *outView,
    uint64_t *outDataOffset) {
  unsigned char riff[12], chunk[8], fmt[40] = {0};
  uint64_t offset = 12, riffEnd;
  size_t fmtSize = 0, dataSize = 0;
  uint64_t dataOffset = 0;
  bool haveFmt = false, haveData = false;
  OPSTATUS status;
  if (!reader || !reader->read || !reader->seek || !outView || !outDataOffset)
    return STATUS_CONST(INVALID_ARGUMENT);
  status = reader->seek(reader->userData, 0, AUDIO_CODEC_CONST(SEEK_START));
  if (status != STATUS_CONST(SUCCESS)) return status;
  status = AUDIO_INS(WAV_ReadExact)(reader, riff, sizeof riff);
  if (status != STATUS_CONST(SUCCESS)) return status;
  if (memcmp(riff, "RIFF", 4) || memcmp(riff + 8, "WAVE", 4))
    return STATUS_CONST(NOT_SUPPORTED);
  if (AUDIO_INS(WAV_U32)(riff + 4) < 4)
    return STATUS_CONST(INVALID_ARGUMENT);
  riffEnd = (uint64_t)AUDIO_INS(WAV_U32)(riff + 4) + 8;
  while (offset < riffEnd) {
    size_t bytes, skip;
    if (riffEnd - offset < 8) return STATUS_CONST(INVALID_ARGUMENT);
    status = reader->seek(reader->userData, (int64_t)offset,
                          AUDIO_CODEC_CONST(SEEK_START));
    if (status != STATUS_CONST(SUCCESS)) return status;
    status = AUDIO_INS(WAV_ReadExact)(reader, chunk, sizeof chunk);
    if (status != STATUS_CONST(SUCCESS)) return status;
    bytes = (size_t)AUDIO_INS(WAV_U32)(chunk + 4);
    offset += 8;
    if (bytes > riffEnd - offset) return STATUS_CONST(INVALID_ARGUMENT);
    if (!haveFmt && !memcmp(chunk, "fmt ", 4)) {
      if (bytes < 16) return STATUS_CONST(INVALID_ARGUMENT);
      fmtSize = bytes < sizeof fmt ? bytes : sizeof fmt;
      status = AUDIO_INS(WAV_ReadExact)(reader, fmt, fmtSize);
      if (status != STATUS_CONST(SUCCESS)) return status;
      haveFmt = true;
    } else if (!haveData && !memcmp(chunk, "data", 4)) {
      dataOffset = offset;
      dataSize = bytes;
      haveData = true;
    }
    skip = bytes;
    if ((bytes & 1U) && bytes < riffEnd - offset) ++skip;
    offset += skip;
  }
  if (!haveFmt || !haveData) return STATUS_CONST(INVALID_ARGUMENT);
  status = AUDIO_INS(WAV_Format)(fmt, fmtSize, dataSize, outView);
  if (status != STATUS_CONST(SUCCESS)) return status;
  *outDataOffset = dataOffset;
  return STATUS_CONST(SUCCESS);
}

/* Use bounded raw chunks; do not allocate a copy of the full recording. */
static inline OPSTATUS AUDIO_INS(WAV_StreamFrames)(
    AUDIO_INS(Decoder) *decoder, float *outSamples,
    size_t capacity, size_t *outFrames) {
  unsigned char raw[4096];
  size_t count = 0;
  const size_t align = decoder->wav.channels * (decoder->wav.bits / 8);
  if (capacity > SIZE_MAX / decoder->wav.channels / sizeof(float))
    return STATUS_CONST(ARITHMETIC_OVERFLOW);
  while (capacity && decoder->wavCursor < decoder->wav.frameCount) {
    size_t frames = sizeof(raw) / align, converted = 0;
    AUDIO_INS(TWAVView) segment = decoder->wav;
    uint64_t position;
    OPSTATUS status;
    if (frames > capacity) frames = capacity;
    if ((uint64_t)frames > decoder->wav.frameCount - decoder->wavCursor)
      frames = (size_t)(decoder->wav.frameCount - decoder->wavCursor);
    position = decoder->wavDataOffset + decoder->wavCursor * align;
    if (position > INT64_MAX) {
      *outFrames = count;
      return STATUS_CONST(OUT_OF_RANGE);
    }
    status = decoder->reader.seek(decoder->reader.userData, (int64_t)position,
                                  AUDIO_CODEC_CONST(SEEK_START));
    if (status == STATUS_CONST(SUCCESS))
      status = AUDIO_INS(WAV_ReadExact)(&decoder->reader, raw, frames * align);
    if (status != STATUS_CONST(SUCCESS)) {
      *outFrames = count;
      return status;
    }
    segment.data = raw;
    segment.byteCount = frames * align;
    segment.frameCount = frames;
    status = AUDIO_INS(WAV_ReadRange)(&segment, 0,
        outSamples + count * decoder->wav.channels, frames, &converted);
    decoder->wavCursor += converted;
    count += converted;
    capacity -= converted;
    if (status != STATUS_CONST(SUCCESS)) {
      *outFrames = count;
      return status;
    }
  }
  *outFrames = count;
  return STATUS_CONST(SUCCESS);
}

static inline OPSTATUS AUDIO_INS(WAV_FileRead)(
    void *user, void *data, size_t capacity, size_t *read) {
  FILE *file = (FILE *)user;
  *read = fread(data, 1, capacity, file);
  return ferror(file) ? STATUS_CONST(GENERIC_ERROR) : STATUS_CONST(SUCCESS);
}
static inline OPSTATUS AUDIO_INS(WAV_FileSeek)(
    void *user, int64_t offset, AUDIO_CODEC_TYPE(TSeekOrigin) origin) {
  int how = origin == AUDIO_CODEC_CONST(SEEK_START) ? SEEK_SET :
            origin == AUDIO_CODEC_CONST(SEEK_CURRENT) ? SEEK_CUR : SEEK_END;
  if (offset > LONG_MAX || offset < LONG_MIN) return STATUS_CONST(OUT_OF_RANGE);
  return fseek((FILE *)user, (long)offset, how) == 0 ?
      STATUS_CONST(SUCCESS) : STATUS_CONST(GENERIC_ERROR);
}
