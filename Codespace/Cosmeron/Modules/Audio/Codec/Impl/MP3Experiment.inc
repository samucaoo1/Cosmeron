#pragma once

/* Experimental MPEG Layer III native frame decode. All MPEG-1 1,152
 * or MPEG-2/2.5 576 frames are staged before publication. The caller's state, PCM output,
 * metadata and frame count stay intact if the frame is malformed.
 * Exception: INCOMPLETE_SEQUENCE for missing pre-roll advances only
 * the reservoir history so subsequent frames can become decodable. */
AUDIO_CODEC_MP3_EXPERIMENT_FRAME_PROTOTYPE {
  AUDIO_CODEC_TYPE(TMP3Experimental) work;
  AUDIO_CODEC_TYPE(TMP3FrameInfo) header;
  AUDIO_CODEC_TYPE(TMP3SideInfo) side;
  AUDIO_CODEC_TYPE(TMP3Scalefactors) previous[2], scaleByChannel[2];
  float stage[1152 * 2], channels[2][576];
  const unsigned char *mainData = NULL;
  size_t mainBytes = 0, produced, i;
  unsigned int gr, ch;
  uint8_t lsfIntensityScale = 0;
  OPSTATUS status;
  if (!state || !frame || !outInterleaved || !outFrames || !outInfo)
    return STATUS_CONST(INVALID_ARGUMENT);
  status = AUDIO_CODEC_FUNC(MP3_ParseHeader)(frame, size, &header);
  if (status != STATUS_CONST(SUCCESS)) return status;
  /* 8 kHz is tested against the reference in the codec CI; promotion
   * depends on parity across the entire stream, not parser success. */
  if (frameCapacity < header.samplesPerFrame)
    return STATUS_CONST(OUT_OF_RANGE);
  if ((state->sampleRate && state->sampleRate != header.sampleRate) ||
      (state->channels && state->channels != header.channels))
    return STATUS_CONST(INVALID_SEQUENCE);
  if (size < header.frameBytes)
    return STATUS_CONST(INCOMPLETE_SEQUENCE);
  memset(previous, 0, sizeof previous);
  memset(scaleByChannel, 0, sizeof scaleByChannel);
  work = *state;
  status = AUDIO_CODEC_FUNC(MP3_ReservoirFeed)(&work.reservoir,
      frame, header.frameBytes, &mainData, &mainBytes, &side);
  if (status == STATUS_CONST(INCOMPLETE_SEQUENCE)) {
    /* This early frame might reference bytes preceding our starting
     * position. Keep its transmitted payload for future frames. */
    state->reservoir = work.reservoir;
    return status;
  }
  if (status != STATUS_CONST(SUCCESS)) return status;
  if (side.channels != header.channels ||
      side.granules != (header.mpegVersion == 1 ? 2U : 1U))
    return STATUS_CONST(INVALID_SEQUENCE);
  for (gr = 0; gr < side.granules; ++gr) {
    for (ch = 0; ch < header.channels; ++ch) {
      AUDIO_CODEC_TYPE(TMP3Scalefactors) scale;
      int32_t coefficients[576];
      size_t written = 0;
      if (header.mpegVersion == 1) {
        status = AUDIO_CODEC_FUNC(MP3_ParseScalefactors)(
            mainData, mainBytes, &side, gr, ch,
            gr ? &previous[ch] : NULL, &scale);
      } else {
        uint8_t intensityScale = 0;
        status = AUDIO_CODEC_FUNC(MP3_ParseScalefactorsLSF)(
            mainData, mainBytes, &header, &side, ch,
            &scale, &intensityScale);
        if (ch == 1) lsfIntensityScale = intensityScale;
      }
      if (status != STATUS_CONST(SUCCESS)) return status;
      if (gr == 0) previous[ch] = scale;
      scaleByChannel[ch] = scale;
      status = AUDIO_CODEC_FUNC(MP3_HuffmanGranule)(
          mainData, mainBytes, &side, &scale, gr, ch,
          header.sampleRate, coefficients, &written);
      if (status != STATUS_CONST(SUCCESS)) return status;
      status = AUDIO_CODEC_FUNC(MP3_Requantize)(
          coefficients, &side.parts[gr][ch], &scale,
          header.sampleRate, channels[ch]);
      if (status != STATUS_CONST(SUCCESS)) return status;
    }
    /* Stereo processing belongs after requantization, before spectral
     * reordering, alias reduction and the hybrid synthesis. */
    if (header.channels == 2 &&
        (header.msStereo || header.intensityStereo)) {
      if (header.mpegVersion == 1)
        status = AUDIO_CODEC_FUNC(MP3_JointStereo)(
            &header, &side.parts[gr][1], &scaleByChannel[1],
            channels[0], channels[1], channels[0], channels[1]);
      else if (header.intensityStereo)
        status = AUDIO_CODEC_FUNC(MP3_JointStereoLSF)(
            &header, &side.parts[gr][1], &scaleByChannel[1],
            lsfIntensityScale, channels[0], channels[1],
            channels[0], channels[1]);
      else
        status = AUDIO_CODEC_FUNC(MP3_StereoMS)(
            channels[0], channels[1], 0, 576,
            channels[0], channels[1]);
      if (status != STATUS_CONST(SUCCESS)) return status;
    }
    for (ch = 0; ch < header.channels; ++ch) {
      float subband[576];
      /* 8 kHz mixed blocks contain 72 long spectral coefficients
       * (four subbands); other MPEG versions use 36 (two subbands). */
      status = AUDIO_CODEC_FUNC(MP3_ReorderShort)(
          channels[ch], &side.parts[gr][ch],
          header.sampleRate, channels[ch]);
      if (status != STATUS_CONST(SUCCESS)) return status;
      status = AUDIO_INS(MP3_AntialiasMixedBoundaries)(
          channels[ch], &side.parts[gr][ch], channels[ch],
          header.sampleRate == 8000 ? 3U : 1U);
      if (status != STATUS_CONST(SUCCESS)) return status;
      status = AUDIO_INS(MP3_IMDCTMixedSubbands)(
          channels[ch], &side.parts[gr][ch],
          &work.hybrid[ch], subband,
          header.sampleRate == 8000 ? 4U : 2U);
      if (status != STATUS_CONST(SUCCESS)) return status;
      status = AUDIO_CODEC_FUNC(MP3_FrequencyInvert)(
          subband, subband);
      if (status != STATUS_CONST(SUCCESS)) return status;
      status = AUDIO_CODEC_FUNC(MP3_Synthesize)(
          subband, &work.synthesis[ch], channels[ch]);
      if (status != STATUS_CONST(SUCCESS)) return status;
    }
    for (i = 0; i < 576; ++i) {
      size_t position = (size_t)gr * 576U * header.channels +
                        i * header.channels;
      for (ch = 0; ch < header.channels; ++ch)
        stage[position + ch] = channels[ch][i];
    }
  }
  produced = (size_t)header.samplesPerFrame * header.channels;
  memcpy(outInterleaved, stage, produced * sizeof(float));
  work.sampleRate = header.sampleRate;
  work.channels = header.channels;
  *state = work;
  *outFrames = header.samplesPerFrame;
  *outInfo = header;
  return STATUS_CONST(SUCCESS);
}
