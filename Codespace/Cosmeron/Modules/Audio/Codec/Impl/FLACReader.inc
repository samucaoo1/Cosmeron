#pragma once

/* Seekable FLAC source: parse metadata with a fixed 42-byte scratch buffer.
 * Other metadata blocks are skipped by absolute seek, not copied into RAM. */
static inline OPSTATUS AUDIO_INS(FLAC_ParseReader)(
    const AUDIO_CODEC_TYPE(TReader) *reader,
    AUDIO_INS(TFLACStreamInfo) *outInfo) {
  unsigned char first[42], header[4], tail;
  size_t offset = 42;
  OPSTATUS status;
  bool last;
  if (!reader || !reader->read || !reader->seek || !outInfo)
    return STATUS_CONST(INVALID_ARGUMENT);
  status = reader->seek(reader->userData, 0, AUDIO_CODEC_CONST(SEEK_START));
  if (status != STATUS_CONST(SUCCESS)) return status;
  status = AUDIO_INS(WAV_ReadExact)(reader, first, 4);
  if (status != STATUS_CONST(SUCCESS)) return status;
  if (memcmp(first, "fLaC", 4)) return STATUS_CONST(NOT_SUPPORTED);
  status = AUDIO_INS(WAV_ReadExact)(reader, first + 4, 38);
  if (status != STATUS_CONST(SUCCESS)) return status;
  if ((first[4] & 0x7fU) != 0 ||
      AUDIO_INS(FLAC_BigEndian24)(first + 5) != 34)
    return STATUS_CONST(INVALID_SEQUENCE);
  last = (first[4] & 0x80U) != 0;
  first[4] |= 0x80U;
  status = AUDIO_INS(FLAC_ParseMetadata)(first, sizeof first, outInfo);
  if (status != STATUS_CONST(SUCCESS)) return status;
  while (!last) {
    uint32_t length;
    if (offset > INT64_MAX) return STATUS_CONST(OUT_OF_RANGE);
    status = reader->seek(reader->userData, (int64_t)offset,
                          AUDIO_CODEC_CONST(SEEK_START));
    if (status != STATUS_CONST(SUCCESS)) return status;
    status = AUDIO_INS(WAV_ReadExact)(reader, header, sizeof header);
    if (status != STATUS_CONST(SUCCESS)) return status;
    if ((header[0] & 0x7fU) == 0 || (header[0] & 0x7fU) == 127)
      return STATUS_CONST(INVALID_SEQUENCE);
    last = (header[0] & 0x80U) != 0;
    length = AUDIO_INS(FLAC_BigEndian24)(header + 1);
    if (offset > SIZE_MAX - 4 - length ||
        offset > INT64_MAX - 4 - length)
      return STATUS_CONST(OUT_OF_RANGE);
    offset += 4 + length;
  }
  /* Check that a seek past EOF did not silently accept a truncated block. */
  status = reader->seek(reader->userData, (int64_t)(offset - 1),
                        AUDIO_CODEC_CONST(SEEK_START));
  if (status != STATUS_CONST(SUCCESS)) return status;
  status = AUDIO_INS(WAV_ReadExact)(reader, &tail, 1);
  if (status != STATUS_CONST(SUCCESS)) return status;
  outInfo->frameOffset = offset;
  return STATUS_CONST(SUCCESS);
}

#define AUDIO_FLAC_MAX_COMPRESSED_FRAME (8U * 1024U * 1024U)

/* Decode exactly one frame, repeatedly extending the bounded compressed
 * window until CRC verification succeeds or the seekable stream ends.
 * No whole-file buffering is required even for large audio recordings. */
static inline OPSTATUS AUDIO_INS(FLAC_StreamNextFrame)(
    AUDIO_INS(Decoder) *decoder, size_t *outFrames) {
  size_t limit = AUDIO_FLAC_MAX_COMPRESSED_FRAME, target = 4096, count = 0;
  bool eof = false;
  OPSTATUS status;
  if (!decoder || !outFrames || !decoder->flacStreaming)
    return STATUS_CONST(INVALID_ARGUMENT);
  *outFrames = 0;
  if (decoder->flacOffset > INT64_MAX) return STATUS_CONST(OUT_OF_RANGE);
  if (decoder->flac.maximumBlockSize > SIZE_MAX / decoder->flac.channels / 8)
    return STATUS_CONST(ARITHMETIC_OVERFLOW);
  if (decoder->flac.minimumBlockSize == 0 || decoder->flac.maximumBlockSize == 0)
    return STATUS_CONST(INVALID_SEQUENCE);
  if (target > limit) target = limit;
  if (decoder->flacEncodedCapacity < target) {
    unsigned char *newBytes = (unsigned char *)realloc(decoder->flacEncoded, target);
    if (!newBytes) return STATUS_CONST(OUT_OF_MEMORY);
    decoder->flacEncoded = newBytes;
    decoder->flacEncodedCapacity = target;
  }
  status = decoder->reader.seek(decoder->reader.userData,
      (int64_t)decoder->flacOffset, AUDIO_CODEC_CONST(SEEK_START));
  if (status != STATUS_CONST(SUCCESS)) return status;
  for (;;) {
    size_t frames = 0, next = 0;
    while (!eof && count < target) {
      size_t received = 0;
      status = decoder->reader.read(decoder->reader.userData,
          decoder->flacEncoded + count, target - count, &received);
      if (status != STATUS_CONST(SUCCESS)) return status;
      if (received > target - count) return STATUS_CONST(INVALID_SEQUENCE);
      if (!received) { eof = true; break; }
      count += received;
    }
    if (!count && eof) return STATUS_CONST(SUCCESS);
    status = AUDIO_INS(FLAC_DecodeFrame)(decoder->flacEncoded, count,
        &decoder->flac, 0, decoder->flacBlock,
        decoder->flac.maximumBlockSize, &frames, &next);
    if (status == STATUS_CONST(SUCCESS)) {
      if (!frames || !next || next > count ||
          next > SIZE_MAX - decoder->flacOffset)
        return STATUS_CONST(INVALID_SEQUENCE);
      decoder->flacOffset += next;
      decoder->flacBlockFrames = frames;
      decoder->flacBlockOffset = 0;
      *outFrames = frames;
      return STATUS_CONST(SUCCESS);
    }
    if (status == STATUS_CONST(OUT_OF_MEMORY)) return status;
    if (eof || target >= limit)
      return STATUS_CONST(INVALID_SEQUENCE);
    if (target > limit / 2) target = limit;
    else target *= 2;
    if (decoder->flacEncodedCapacity < target) {
      unsigned char *newBytes = (unsigned char *)realloc(
          decoder->flacEncoded, target);
      if (!newBytes) return STATUS_CONST(OUT_OF_MEMORY);
      decoder->flacEncoded = newBytes;
      decoder->flacEncodedCapacity = target;
    }
  }
}
