#pragma once

/* Xing/Info VBR frame index and optional encoder delay/padding metadata.
 * The metadata itself does not change the bitstream decoder state.
 * Decoder-specific synthesis priming (usually 529 samples in Layer III)
 * must be accounted for when mapping raw decode to playback samples. */
AUDIO_CODEC_MP3_PARSE_XING_PROTOTYPE {
  const unsigned char *p = (const unsigned char *)frame;
  AUDIO_CODEC_TYPE(TMP3FrameInfo) header;
  AUDIO_CODEC_TYPE(TMP3XingInfo) result = {0};
  size_t at, tag;
  unsigned int flags;
  OPSTATUS status;
  if (!p || !outInfo) return STATUS_CONST(INVALID_ARGUMENT);
  status = AUDIO_CODEC_FUNC(MP3_ParseHeader)(frame, length, &header);
  if (status != STATUS_CONST(SUCCESS)) return status;
  if (length < header.frameBytes) return STATUS_CONST(INCOMPLETE_SEQUENCE);
  at = 4U + (header.crcProtected ? 2U : 0U) + header.sideInfoBytes;
  if (at > header.frameBytes || header.frameBytes - at < 8U)
    return STATUS_CONST(NOT_FOUND);
  if (memcmp(p + at, "Xing", 4) && memcmp(p + at, "Info", 4))
    return STATUS_CONST(NOT_FOUND);
  result.variableBitrate = memcmp(p + at, "Xing", 4) == 0;
  flags = ((unsigned int)p[at+4] << 24) | ((unsigned int)p[at+5] << 16) |
          ((unsigned int)p[at+6] << 8) | p[at+7];
  if (flags & ~15U) return STATUS_CONST(INVALID_SEQUENCE);
  result.flags = (uint8_t)flags;
  at += 8;
  if (flags & 1U) {
    if (header.frameBytes - at < 4) return STATUS_CONST(INCOMPLETE_SEQUENCE);
    result.encodedFrames = ((uint32_t)p[at] << 24) |
        ((uint32_t)p[at+1] << 16) | ((uint32_t)p[at+2] << 8) | p[at+3];
    at += 4;
  }
  if (flags & 2U) {
    if (header.frameBytes - at < 4) return STATUS_CONST(INCOMPLETE_SEQUENCE);
    result.encodedBytes = ((uint32_t)p[at] << 24) |
        ((uint32_t)p[at+1] << 16) | ((uint32_t)p[at+2] << 8) | p[at+3];
    at += 4;
  }
  if (flags & 4U) {
    if (header.frameBytes - at < 100) return STATUS_CONST(INCOMPLETE_SEQUENCE);
    result.hasTOC = true;
    at += 100;
  }
  if (flags & 8U) {
    if (header.frameBytes - at < 4) return STATUS_CONST(INCOMPLETE_SEQUENCE);
    result.vbrScale = ((uint32_t)p[at] << 24) |
        ((uint32_t)p[at+1] << 16) | ((uint32_t)p[at+2] << 8) | p[at+3];
    at += 4;
  }
  /* LAME-family extensions hold 12-bit delay and padding at 21..23.
   * Metadata positions are relative to the 36-byte extension start. */
  tag = at;
  if (header.frameBytes - tag >= 24U &&
      (!memcmp(p + tag, "LAME", 4) ||
       !memcmp(p + tag, "Lavf", 4) ||
       !memcmp(p + tag, "Lavc", 4))) {
    result.encoderDelay = ((uint16_t)p[tag+21] << 4) | (p[tag+22] >> 4);
    result.endPadding = ((uint16_t)(p[tag+22] & 15U) << 8) | p[tag+23];
    result.hasGapless = true;
  }
  if (result.encodedFrames &&
      (uint64_t)result.encoderDelay + result.endPadding >
          (uint64_t)result.encodedFrames * header.samplesPerFrame)
    return STATUS_CONST(INVALID_SEQUENCE);
  *outInfo = result;
  return STATUS_CONST(SUCCESS);
}

/* Map a raw native MP3 decode to gapless presentation samples. The
 * caller explicitly supplies the number of leading Xing/Info frames
 * excluded from encodedFrames (usually one for LAME), and must have
 * decoded that many frames before indexing the returned start. */
AUDIO_CODEC_MP3_GAPLESS_SPAN_PROTOTYPE {
  uint64_t encoded, raw, first, presentation;
  if (!xing || !outFirstPCM || !outFrames ||
      (samplesPerFrame != 1152 && samplesPerFrame != 576))
    return STATUS_CONST(INVALID_ARGUMENT);
  if (!xing->hasGapless || !xing->encodedFrames)
    return STATUS_CONST(NOT_FOUND);
  encoded = (uint64_t)xing->encodedFrames * samplesPerFrame;
  raw = ((uint64_t)xing->encodedFrames + metadataFrames) * samplesPerFrame;
  if ((uint64_t)xing->encoderDelay + xing->endPadding > encoded)
    return STATUS_CONST(INVALID_SEQUENCE);
  /* Encoders writing zero/short end padding still require synthesis
   * priming. Do not expose samples beyond the complete raw decode. */
  {
    uint64_t tail = xing->endPadding < AUDIO_CODEC_CONST(MP3_DECODER_PRIMING) ?
        AUDIO_CODEC_CONST(MP3_DECODER_PRIMING) : xing->endPadding;
    if ((uint64_t)xing->encoderDelay + tail > encoded)
      return STATUS_CONST(INVALID_SEQUENCE);
    presentation = encoded - xing->encoderDelay - tail;
  }
  first = (uint64_t)metadataFrames * samplesPerFrame +
      xing->encoderDelay + AUDIO_CODEC_CONST(MP3_DECODER_PRIMING);
  if (first > raw || presentation > raw - first)
    return STATUS_CONST(NOT_SUPPORTED);
  *outFirstPCM = first;
  *outFrames = presentation;
  return STATUS_CONST(SUCCESS);
}
