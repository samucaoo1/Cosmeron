#pragma once

typedef struct AUDIO_INS(TMP3MemoryDecoder) {
  AUDIO_CODEC_TYPE(TMP3Experimental) decoder;
  const unsigned char *source;
  size_t size, firstByte, lastByte, nextByte;
  uint64_t rawDecoded, rawFirst, frameCount, presented, stagedFirst;
  uint32_t rate, channels;
  uint16_t samplesPerFrame;
  size_t stagedFrames, stagedCursor;
  float stage[1152 * 2];
} AUDIO_INS(TMP3MemoryDecoder);

/* A complete frame is either published to the staging buffer or leaves
 * the synthesis state intact. MPEG reservoir preroll is allowed only
 * inside a metadata-confirmed trimmed prefix. */
static inline OPSTATUS AUDIO_INS(MP3_MemoryConsume)(
    AUDIO_INS(TMP3MemoryDecoder) *value, float *dst, size_t capacity,
    size_t *outCount) {
  size_t done = 0;
  OPSTATUS status = STATUS_CONST(SUCCESS);
  if (!value || !outCount) return STATUS_CONST(INVALID_ARGUMENT);
  while (done < capacity && value->presented < value->frameCount) {
    uint64_t rawEnd = value->rawFirst + value->frameCount;
    size_t available, n;
    if (value->stagedCursor == value->stagedFrames) {
      AUDIO_CODEC_TYPE(TMP3FrameInfo) header;
      size_t decoded = 0;
      if (value->nextByte >= value->lastByte) {
        status = STATUS_CONST(INCOMPLETE_SEQUENCE);
        break;
      }
      status = AUDIO_CODEC_FUNC(MP3_ParseHeader)(
          value->source + value->nextByte,
          value->lastByte - value->nextByte, &header);
      if (status != STATUS_CONST(SUCCESS)) break;
      if (header.frameBytes > value->lastByte - value->nextByte ||
          header.sampleRate != value->rate ||
          header.channels != value->channels ||
          header.samplesPerFrame != value->samplesPerFrame) {
        status = STATUS_CONST(INVALID_SEQUENCE);
        break;
      }
      status = AUDIO_CODEC_FUNC(MP3_ExperimentFrame)(
          &value->decoder, value->source + value->nextByte,
          header.frameBytes, value->stage, 1152,
          &decoded, &header);
      if (status != STATUS_CONST(SUCCESS) &&
          status != STATUS_CONST(INCOMPLETE_SEQUENCE)) break;
      if (status == STATUS_CONST(SUCCESS) &&
          decoded != value->samplesPerFrame) {
        status = STATUS_CONST(INVALID_SEQUENCE);
        break;
      }
      value->stagedFirst = value->rawDecoded;
      value->rawDecoded += value->samplesPerFrame;
      value->nextByte += header.frameBytes;
      if (status == STATUS_CONST(INCOMPLETE_SEQUENCE)) {
        if (value->rawDecoded > value->rawFirst) break;
        value->stagedFrames = 0;
        value->stagedCursor = 0;
        status = STATUS_CONST(SUCCESS);
        continue;
      }
      value->stagedFrames = decoded;
      value->stagedCursor = 0;
      if (value->stagedFirst < value->rawFirst) {
        uint64_t omit = value->rawFirst - value->stagedFirst;
        value->stagedCursor = omit < decoded ? (size_t)omit : decoded;
      }
    }
    if (value->stagedCursor == value->stagedFrames) continue;
    if (value->stagedFirst + value->stagedCursor >= rawEnd) break;
    available = value->stagedFrames - value->stagedCursor;
    if (available > capacity - done) available = capacity - done;
    if ((uint64_t)available >
        rawEnd - (value->stagedFirst + value->stagedCursor))
      available = (size_t)(rawEnd - (value->stagedFirst + value->stagedCursor));
    n = available;
    if (n == 0) break;
    if (dst) memcpy(dst + done * value->channels,
        value->stage + value->stagedCursor * value->channels,
        n * value->channels * sizeof(float));
    value->stagedCursor += n;
    value->presented += n;
    done += n;
  }
  *outCount = done;
  return status;
}

AUDIO_CODEC_MP3_MEMORY_OPEN_PROTOTYPE {
  const unsigned char *data = (const unsigned char *)bytes;
  AUDIO_CODEC_TYPE(TMP3FrameInfo) first, current;
  AUDIO_CODEC_TYPE(TMP3XingInfo) xing = {0};
  AUDIO_INS(TMP3MemoryDecoder) *value;
  OPSTATUS status;
  size_t beginning = 0, offset, count = 0;
  uint64_t rawFrames, presentation = 0, rawFirst = 0;
  if (!stream || !bytes || !size) return STATUS_CONST(INVALID_ARGUMENT);
  if (stream->internal) return STATUS_CONST(BUSY);
  status = AUDIO_CODEC_FUNC(MP3_FindFrame)(
      bytes, size, 0, &beginning, &first);
  if (status != STATUS_CONST(SUCCESS)) return status;
  offset = beginning;
  while (size - offset >= 4) {
    status = AUDIO_CODEC_FUNC(MP3_ParseHeader)(
        data + offset, size - offset, &current);
    if (status != STATUS_CONST(SUCCESS)) break;
    if (current.frameBytes > size - offset)
      return STATUS_CONST(INCOMPLETE_SEQUENCE);
    if (current.sampleRate != first.sampleRate ||
        current.channels != first.channels ||
        current.mpegVersion != first.mpegVersion ||
        current.samplesPerFrame != first.samplesPerFrame)
      return STATUS_CONST(INVALID_SEQUENCE);
    if (++count > UINT64_MAX / first.samplesPerFrame)
      return STATUS_CONST(ARITHMETIC_OVERFLOW);
    offset += current.frameBytes;
  }
  if (count < 2) return STATUS_CONST(NOT_SUPPORTED);
  /* Permit standard ID3v1 footer; reject interrupted or unknown audio
   * segments rather than silently truncating the file's duration. */
  if (offset != size &&
      (size - offset != 128 || memcmp(data + offset, "TAG", 3)))
    return STATUS_CONST(INVALID_SEQUENCE);
  rawFrames = (uint64_t)count * first.samplesPerFrame;
  presentation = rawFrames;
  if (AUDIO_CODEC_FUNC(MP3_ParseXing)(
      data + beginning, size - beginning, &xing) ==
      STATUS_CONST(SUCCESS) && xing.hasGapless && xing.encodedFrames &&
      count >= xing.encodedFrames && count - xing.encodedFrames <= 2) {
    uint64_t length = 0;
    status = AUDIO_CODEC_FUNC(MP3_GaplessSpan)(
        &xing, first.samplesPerFrame,
        (uint32_t)(count - xing.encodedFrames), &rawFirst, &length);
    if (status == STATUS_CONST(SUCCESS)) presentation = length;
  }
  value = (AUDIO_INS(TMP3MemoryDecoder) *)calloc(1, sizeof(*value));
  if (!value) return STATUS_CONST(OUT_OF_MEMORY);
  value->source = data;
  value->size = size;
  value->firstByte = beginning;
  value->lastByte = offset;
  value->nextByte = beginning;
  value->rawFirst = rawFirst;
  value->frameCount = presentation;
  value->rate = first.sampleRate;
  value->channels = first.channels;
  value->samplesPerFrame = first.samplesPerFrame;
  stream->internal = value;
  return STATUS_CONST(SUCCESS);
}

AUDIO_CODEC_MP3_MEMORY_INFO_PROTOTYPE {
  const AUDIO_INS(TMP3MemoryDecoder) *value;
  if (!stream || !stream->internal || !outRate || !outChannels || !outFrames)
    return STATUS_CONST(INVALID_ARGUMENT);
  value = (const AUDIO_INS(TMP3MemoryDecoder) *)stream->internal;
  *outRate = value->rate;
  *outChannels = value->channels;
  *outFrames = value->frameCount;
  return STATUS_CONST(SUCCESS);
}

AUDIO_CODEC_MP3_MEMORY_READ_PROTOTYPE {
  AUDIO_INS(TMP3MemoryDecoder) *value;
  if (!stream || !stream->internal || !outFrames ||
      (capacity && !outInterleaved))
    return STATUS_CONST(INVALID_ARGUMENT);
  value = (AUDIO_INS(TMP3MemoryDecoder) *)stream->internal;
  if (capacity > SIZE_MAX / value->channels / sizeof(float))
    return STATUS_CONST(ARITHMETIC_OVERFLOW);
  return AUDIO_INS(MP3_MemoryConsume)(value, outInterleaved, capacity, outFrames);
}

AUDIO_CODEC_MP3_MEMORY_SEEK_PROTOTYPE {
  AUDIO_INS(TMP3MemoryDecoder) *value, *scratch;
  uint64_t remain;
  if (!stream || !stream->internal)
    return STATUS_CONST(INVALID_ARGUMENT);
  value = (AUDIO_INS(TMP3MemoryDecoder) *)stream->internal;
  if (frameIndex > value->frameCount) return STATUS_CONST(OUT_OF_RANGE);
  if (frameIndex == value->presented) return STATUS_CONST(SUCCESS);
  /* The compressed decoder state is already at the end of the staged
   * PCM frame. A seek inside that frame requires no additional decode,
   * I/O or allocation, even when moving backwards. */
  if (value->stagedFrames) {
    uint64_t rawTarget = value->rawFirst + frameIndex;
    uint64_t stagedEnd = value->stagedFirst + value->stagedFrames;
    if (rawTarget >= value->stagedFirst && rawTarget <= stagedEnd) {
      value->stagedCursor = (size_t)(rawTarget - value->stagedFirst);
      value->presented = frameIndex;
      return STATUS_CONST(SUCCESS);
    }
  }
  /* A forward seek can reuse the entire reservoir/overlap/synthesis
   * history. Backward seeks outside the staged frame must replay from
   * the beginning. Either path is transactional on failure. */
  scratch = (AUDIO_INS(TMP3MemoryDecoder) *)malloc(sizeof(*scratch));
  if (!scratch) return STATUS_CONST(OUT_OF_MEMORY);
  *scratch = *value;
  if (frameIndex < value->presented) {
    memset(&scratch->decoder, 0, sizeof scratch->decoder);
    scratch->nextByte = value->firstByte;
    scratch->rawDecoded = scratch->stagedFirst = scratch->presented = 0;
    scratch->stagedCursor = scratch->stagedFrames = 0;
  }
  remain = frameIndex - scratch->presented;
  while (remain) {
    size_t got = 0;
    size_t n = remain > 1152 ? 1152 : (size_t)remain;
    OPSTATUS status = AUDIO_INS(MP3_MemoryConsume)(
        scratch, NULL, n, &got);
    if (status != STATUS_CONST(SUCCESS) || got != n) {
      free(scratch);
      return status == STATUS_CONST(SUCCESS) ?
          STATUS_CONST(INCOMPLETE_SEQUENCE) : status;
    }
    remain -= got;
  }
  *value = *scratch;
  free(scratch);
  return STATUS_CONST(SUCCESS);
}

AUDIO_CODEC_MP3_MEMORY_CLOSE_PROTOTYPE {
  if (stream && stream->internal) {
    free(stream->internal);
    stream->internal = NULL;
  }
}
