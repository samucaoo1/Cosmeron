#pragma once

/* Seekable, caller-owned MP3 Reader. Only a single compressed MPEG frame
 * and a single uncompressed frame are resident; Xing is read from the
 * first frame, not from a copy of the entire file. */
typedef struct AUDIO_INS(TMP3ReaderStream) {
  AUDIO_CODEC_TYPE(TMP3Experimental) decoder;
  size_t firstByte, nextByte, lastByte;
  uint64_t droppedBytes, concealedFrames;
  uint64_t rawDecoded, rawFirst, frameCount, presented, stagedFirst;
  uint32_t rate, channels;
  uint16_t samplesPerFrame;
  struct AUDIO_INS(TMP3ReaderStreamCheckpoint) *checkpoints;
  AUDIO_INS(TMP3SeekIndex) seekIndex;
  size_t checkpointCount;
  uint64_t checkpointStride;
  bool indexingDisabled;
  bool sequential, ended, lengthKnown;
  size_t encodedFilled, pendingResyncBytes;
  size_t stagedFrames, stagedCursor;
  unsigned char encoded[4096];
  float pcm[1152 * 2];
} AUDIO_INS(TMP3ReaderStream);

/* Sparse exact-state checkpoints: reservoir, hybrid overlap and synthesis
 * history must all be restored for sample-identical VBR backward seeks.
 * Lazy population avoids decoding the entire file during open. */
typedef struct AUDIO_INS(TMP3ReaderStreamCheckpoint) {
  AUDIO_INS(TMP3ReaderStream) state;
  struct AUDIO_INS(TMP3ReaderStreamCheckpoint) *next;
} AUDIO_INS(TMP3ReaderStreamCheckpoint);
static inline void AUDIO_INS(TMP3ReaderStream_Remember)(AUDIO_INS(TMP3ReaderStream) *s) {
  AUDIO_INS(TMP3ReaderStreamCheckpoint) *p;
  if (s->sequential || s->indexingDisabled || !s->rawDecoded || s->rawDecoded % (s->checkpointStride * s->samplesPerFrame) ||
      s->checkpointCount >= 1024) return;
  for (p = s->checkpoints; p; p = p->next)
    if (p->state.rawDecoded == s->rawDecoded) return;
  p = (AUDIO_INS(TMP3ReaderStreamCheckpoint) *)malloc(sizeof *p);
  if (!p) return; /* optional optimization; decode remains usable */
  p->state = *s;
  p->next = s->checkpoints;
  s->checkpoints = p;
  ++s->checkpointCount;
}
static inline void AUDIO_INS(TMP3ReaderStream_Forget)(AUDIO_INS(TMP3ReaderStream) *s) {
  AUDIO_INS(TMP3ReaderStreamCheckpoint) *p = s->checkpoints;
  while (p) { AUDIO_INS(TMP3ReaderStreamCheckpoint) *next = p->next; free(p); p = next; }
  free(s->seekIndex.points);
}


/* Distinguish true EOF from a partial four-byte header, including short
 * reads from an arbitrary TReader callback. */
static inline OPSTATUS AUDIO_INS(MP3_ReaderHeader)(
    const AUDIO_CODEC_TYPE(TReader) *reader, size_t position,
    unsigned char bytes[4]) {
  size_t filled = 0;
  OPSTATUS status;
  if (SIZE_MAX > INT64_MAX && position > (size_t)INT64_MAX) return STATUS_CONST(OUT_OF_RANGE);
  status = reader->seek(reader->userData, (int64_t)position,
                        AUDIO_CODEC_CONST(SEEK_START));
  if (status != STATUS_CONST(SUCCESS)) return status;
  while (filled < 4) {
    size_t got = 0;
    status = reader->read(reader->userData, bytes+filled, 4-filled, &got);
    if (status != STATUS_CONST(SUCCESS)) return status;
    if (got > 4-filled) return STATUS_CONST(INVALID_SEQUENCE);
    if (!got) return filled ? STATUS_CONST(INCOMPLETE_SEQUENCE) :
                            STATUS_CONST(NOT_FOUND);
    filled += got;
  }
  return STATUS_CONST(SUCCESS);
}

#if AUDIO_CODEC_MP3_RECOVER_ERRORS
static inline OPSTATUS AUDIO_INS(MP3_ResyncReader)(const AUDIO_CODEC_TYPE(TReader) *reader,
    size_t start,uint32_t rate,uint32_t channels,uint16_t spf,size_t *next,
    unsigned char bytes[4],AUDIO_CODEC_TYPE(TMP3FrameInfo) *header) {
  size_t step;
  for(step=1;step<=AUDIO_CODEC_MP3_MAX_RESYNC_BYTES;++step) {
    OPSTATUS status;
    if(start>SIZE_MAX-step) return STATUS_CONST(OUT_OF_RANGE);
    status=AUDIO_INS(MP3_ReaderHeader)(reader,start+step,bytes);
    if(status!=STATUS_CONST(SUCCESS)) return status;
    if(AUDIO_CODEC_FUNC(MP3_ParseHeader)(bytes,4,header)==STATUS_CONST(SUCCESS) &&
        header->sampleRate==rate && header->channels==channels && header->samplesPerFrame==spf) {
      *next=start+step; return STATUS_CONST(SUCCESS);
    }
  }
  return STATUS_CONST(INVALID_SEQUENCE);
}
#endif

/* Perform a strict full-frame preflight. This is O(file bytes) in I/O
 * volume but uses O(1) memory; no decoded PCM is cached. */
static inline OPSTATUS AUDIO_INS(MP3_ReaderOpen)(
    AUDIO_CODEC_TYPE(TDecoder) *handle,
    const AUDIO_CODEC_TYPE(TReader) *reader, FILE *ownedFile) {
  unsigned char probe[10] = {0}, firstFrame[4096], headerBytes[4];
  AUDIO_CODEC_TYPE(TMP3FrameInfo) first = {0}, current = {0};
  AUDIO_CODEC_TYPE(TMP3XingInfo) xing = {0};
  AUDIO_INS(TMP3ReaderStream) *stream;
  AUDIO_INS(Decoder) *decoder;
  uint64_t frames = 0, firstPCM = 0, presentation;
  size_t beginning = 0, offset = 0, encoded = 0;
  bool found = false, tagged = false, resynced=false;
  OPSTATUS status;
  if (!handle || !reader || !reader->read || !reader->seek)
    return STATUS_CONST(INVALID_ARGUMENT);
  stream = (AUDIO_INS(TMP3ReaderStream) *)calloc(1, sizeof *stream);
  if (!stream) return STATUS_CONST(OUT_OF_MEMORY);
  stream->seekIndex.disabled=AUDIO_CODEC_MP3_RECOVER_ERRORS!=0;
  status = reader->seek(reader->userData, 0, AUDIO_CODEC_CONST(SEEK_START));
  if (status != STATUS_CONST(SUCCESS)) goto fail;
  /* The first 10 bytes suffice to inspect an optional ID3v2 tag. */
  status = AUDIO_INS(WAV_ReadExact)(reader, probe, sizeof probe);
  if (status != STATUS_CONST(SUCCESS)) goto fail;
  /* Use the same strict ten-byte validation as MP3_SkipID3.
   * Validate v2.4 footers with a bounded read, independent of tag size. */
  if (!memcmp(probe, "ID3", 3)) {
    tagged = true;
    status = AUDIO_INS(MP3_ID3Header)(probe, &beginning);
    if (status != STATUS_CONST(SUCCESS)) goto fail;
    if (SIZE_MAX > INT64_MAX && beginning > (size_t)INT64_MAX) { status = STATUS_CONST(OUT_OF_RANGE); goto fail; }
    if (probe[3] == 4 && (probe[5] & 0x10U)) {
      unsigned char footer[10];
      status = reader->seek(reader->userData, (int64_t)(beginning - 10U),
                            AUDIO_CODEC_CONST(SEEK_START));
      if (status != STATUS_CONST(SUCCESS)) goto fail;
      status = AUDIO_INS(WAV_ReadExact)(reader, footer, sizeof footer);
      if (status != STATUS_CONST(SUCCESS)) goto fail;
      if (!AUDIO_INS(MP3_ID3Footer)(probe, footer))
        { status = STATUS_CONST(INVALID_SEQUENCE); goto fail; }
    }
  }
  offset = beginning;
  for (;;) {
    status = AUDIO_INS(MP3_ReaderHeader)(reader, offset, headerBytes);
    if (status == STATUS_CONST(NOT_FOUND)) break;
    if (status != STATUS_CONST(SUCCESS)) goto fail;
    status = AUDIO_CODEC_FUNC(MP3_ParseHeader)(
        headerBytes, sizeof headerBytes, &current);
    if (status != STATUS_CONST(SUCCESS) || (found &&
        (current.sampleRate!=first.sampleRate || current.channels!=first.channels ||
         current.samplesPerFrame!=first.samplesPerFrame))) {
      /* A 128-byte ID3v1 footer is permitted after complete MPEG frames. */
      if (frames && !memcmp(headerBytes, "TAG", 3)) {
        unsigned char trailer[128], check;
        size_t received = 0;
        if (SIZE_MAX > INT64_MAX && offset > (size_t)(INT64_MAX - 128)) { status = STATUS_CONST(OUT_OF_RANGE); goto fail; }
        status = reader->seek(reader->userData, (int64_t)offset,
                              AUDIO_CODEC_CONST(SEEK_START));
        if (status != STATUS_CONST(SUCCESS)) goto fail;
        status = AUDIO_INS(WAV_ReadExact)(reader, trailer, sizeof trailer);
        if (status != STATUS_CONST(SUCCESS)) goto fail;
        status = reader->read(reader->userData, &check, 1, &received);
        if (status != STATUS_CONST(SUCCESS)) goto fail;
        if (received || memcmp(trailer, "TAG", 3))
          { status = STATUS_CONST(INVALID_SEQUENCE); goto fail; }
        break;
      }
#if AUDIO_CODEC_MP3_RECOVER_ERRORS
      if(found) {
        size_t next;
        status=AUDIO_INS(MP3_ResyncReader)(reader,offset,first.sampleRate,first.channels,
            first.samplesPerFrame,&next,headerBytes,&current);
        if(status!=STATUS_CONST(SUCCESS)) goto fail;
        offset=next; resynced=true;
      } else { status=STATUS_CONST(NOT_SUPPORTED); goto fail; }
#else
      { status = found ? STATUS_CONST(INVALID_SEQUENCE) :
          STATUS_CONST(NOT_SUPPORTED); goto fail; }
#endif
    }
    if (!found) {first = current; found = true;}
    if (current.sampleRate != first.sampleRate ||
        current.channels != first.channels ||
        current.mpegVersion != first.mpegVersion ||
        current.samplesPerFrame != first.samplesPerFrame)
      { status = STATUS_CONST(INVALID_SEQUENCE); goto fail; }
    if (current.frameBytes > sizeof firstFrame ||
        (SIZE_MAX > INT64_MAX && offset > (size_t)INT64_MAX - current.frameBytes))
      { status = STATUS_CONST(NOT_SUPPORTED); goto fail; }
    if (frames == 0) {
      status = reader->seek(reader->userData, (int64_t)offset,
                            AUDIO_CODEC_CONST(SEEK_START));
      if (status != STATUS_CONST(SUCCESS)) goto fail;
      status = AUDIO_INS(WAV_ReadExact)(
          reader, firstFrame, current.frameBytes);
      if (status != STATUS_CONST(SUCCESS)) goto fail;
      encoded = current.frameBytes;
    }
    AUDIO_INS(MP3_IndexRecord)(&stream->seekIndex,frames,offset);
    ++frames;
    if (frames > UINT64_MAX / first.samplesPerFrame)
      { status = STATUS_CONST(ARITHMETIC_OVERFLOW); goto fail; }
    offset += current.frameBytes;
  }
  if (!found)
    { status = tagged ? STATUS_CONST(INCOMPLETE_SEQUENCE) :
                    STATUS_CONST(NOT_SUPPORTED); goto fail; }
  if (frames < 2) { status = STATUS_CONST(NOT_SUPPORTED); goto fail; }
  /* Verify final frame payload, not just its 4-byte header. */
  if (!offset || (SIZE_MAX > INT64_MAX && offset > (size_t)INT64_MAX)) { status = STATUS_CONST(OUT_OF_RANGE); goto fail; }
  {
    unsigned char tail;
    status = reader->seek(reader->userData, (int64_t)(offset-1),
                          AUDIO_CODEC_CONST(SEEK_START));
    if (status != STATUS_CONST(SUCCESS)) goto fail;
    status = AUDIO_INS(WAV_ReadExact)(reader, &tail, 1);
    if (status != STATUS_CONST(SUCCESS)) goto fail;
  }
  presentation = frames * first.samplesPerFrame;
  if (!resynced && AUDIO_CODEC_FUNC(MP3_ParseXing)(
      firstFrame, encoded, &xing) == STATUS_CONST(SUCCESS) &&
      xing.hasGapless && xing.encodedFrames &&
      frames >= xing.encodedFrames && frames - xing.encodedFrames <= 2) {
    uint64_t trimmed = 0;
    status = AUDIO_CODEC_FUNC(MP3_GaplessSpan)(
        &xing, first.samplesPerFrame,
        (uint32_t)(frames - xing.encodedFrames), &firstPCM, &trimmed);
    if (status == STATUS_CONST(SUCCESS)) presentation = trimmed;
  }
  decoder = (AUDIO_INS(Decoder) *)calloc(1, sizeof *decoder);
  if (!decoder) { status = STATUS_CONST(OUT_OF_MEMORY); goto fail; }
  stream->firstByte = stream->nextByte = beginning;
  stream->lastByte = offset;
  stream->rate = first.sampleRate;
  stream->channels = first.channels;
  stream->samplesPerFrame = first.samplesPerFrame;
  stream->checkpointStride = frames / 1024 + 1;
  if (stream->checkpointStride < 64) stream->checkpointStride = 64;
  stream->rawFirst = firstPCM;
  stream->frameCount = presentation;
  stream->lengthKnown = true;
  decoder->mp3Stream = stream;
  decoder->nativeMp3Streaming = true;
  decoder->reader = *reader;
  decoder->ownedFile = ownedFile;
  handle->internal = decoder;
  return STATUS_CONST(SUCCESS);
fail:
  AUDIO_INS(TMP3ReaderStream_Forget)(stream); free(stream); return status;
}

/* Forward-only MPEG source. Neither open nor read invokes seek. Length is
 * unknown until EOF unless Xing supplies it. I/O storage is bounded to one
 * MPEG frame; tags are discarded in small chunks rather than retained. */
static inline OPSTATUS AUDIO_INS(MP3_SequentialFill)(
    const AUDIO_CODEC_TYPE(TReader) *r, unsigned char *bytes,
    size_t target, size_t *filled) {
  while (*filled < target) {
    size_t got = 0;
    OPSTATUS status = r->read(r->userData, bytes + *filled, target - *filled, &got);
    if (got > target - *filled) return STATUS_CONST(INVALID_SEQUENCE);
    *filled += got;
    if (status != STATUS_CONST(SUCCESS)) return status;
    if (!got) return *filled ? STATUS_CONST(INCOMPLETE_SEQUENCE) : STATUS_CONST(NOT_FOUND);
  }
  return STATUS_CONST(SUCCESS);
}
static inline OPSTATUS AUDIO_INS(MP3_SequentialOpen)(
    AUDIO_CODEC_TYPE(TDecoder) *handle, const AUDIO_CODEC_TYPE(TReader) *reader) {
  unsigned char probe[10], discard[512];
  size_t filled = 0;
  AUDIO_CODEC_TYPE(TMP3FrameInfo) first;
  AUDIO_CODEC_TYPE(TMP3XingInfo) xing = {0};
  AUDIO_INS(TMP3ReaderStream) *stream;
  AUDIO_INS(Decoder) *decoder;
  OPSTATUS status = AUDIO_INS(MP3_SequentialFill)(reader, probe, 10, &filled);
  if (status != STATUS_CONST(SUCCESS)) return status;
  if (!memcmp(probe, "ID3", 3)) {
    size_t skip, left;
    status = AUDIO_INS(MP3_ID3Header)(probe, &skip);
    if (status != STATUS_CONST(SUCCESS)) return status;
    left = skip - 10;
    while (left) {
      size_t n = left > sizeof discard ? sizeof discard : left;
      /* Keep a possible footer as a complete separate chunk. */
      if (probe[3] == 4 && (probe[5] & 0x10U) && left > 10 && n > left - 10)
        n = left - 10;
      filled = 0;
      status = AUDIO_INS(MP3_SequentialFill)(reader, discard, n, &filled);
      if (status != STATUS_CONST(SUCCESS)) return status;
      if (probe[3] == 4 && (probe[5] & 0x10U) && left == 10 &&
          !AUDIO_INS(MP3_ID3Footer)(probe, discard)) return STATUS_CONST(INVALID_SEQUENCE);
      left -= n;
    }
    filled = 0;
    status = AUDIO_INS(MP3_SequentialFill)(reader, probe, 10, &filled);
    if (status != STATUS_CONST(SUCCESS)) return status;
  }
  status = AUDIO_CODEC_FUNC(MP3_ParseHeader)(probe, 10, &first);
  if (status != STATUS_CONST(SUCCESS)) return status;
  if (first.frameBytes < 10 || first.frameBytes > 4096) return STATUS_CONST(NOT_SUPPORTED);
  stream = (AUDIO_INS(TMP3ReaderStream) *)calloc(1, sizeof *stream);
  decoder = (AUDIO_INS(Decoder) *)calloc(1, sizeof *decoder);
  if (!stream || !decoder) { free(stream); free(decoder); return STATUS_CONST(OUT_OF_MEMORY); }
  memcpy(stream->encoded, probe, 10);
  stream->encodedFilled = 10;
  status = AUDIO_INS(MP3_SequentialFill)(reader, stream->encoded,
      first.frameBytes, &stream->encodedFilled);
  if (status != STATUS_CONST(SUCCESS)) { free(stream); free(decoder); return status; }
  stream->sequential = true;
  stream->nextByte = 0;
  stream->lastByte = SIZE_MAX;
  stream->rate = first.sampleRate;
  stream->channels = first.channels;
  stream->samplesPerFrame = first.samplesPerFrame;
  stream->frameCount = UINT64_MAX;
  if (AUDIO_CODEC_FUNC(MP3_ParseXing)(stream->encoded, first.frameBytes, &xing) ==
      STATUS_CONST(SUCCESS) && xing.hasGapless && xing.encodedFrames) {
    uint64_t length;
    if (AUDIO_CODEC_FUNC(MP3_GaplessSpan)(&xing, first.samplesPerFrame, 1,
        &stream->rawFirst, &length) == STATUS_CONST(SUCCESS)) {
      stream->frameCount = length;
      stream->lengthKnown = true;
    }
  }
  decoder->mp3Stream = stream;
  decoder->nativeMp3Streaming = true;
  decoder->reader = *reader;
  handle->internal = decoder;
  return STATUS_CONST(SUCCESS);
}

/* Read complete encoded frames by absolute offset so seeks are independent
 * of external FILE/callback cursor after either successful or failed I/O. */
static inline OPSTATUS AUDIO_INS(MP3_ReaderConsume)(
    const AUDIO_CODEC_TYPE(TReader) *reader,
    AUDIO_INS(TMP3ReaderStream) *stream,
    float *samples, size_t capacity, size_t *outFrames) {
  size_t done = 0;
  OPSTATUS status = STATUS_CONST(SUCCESS);
  if (!stream || !outFrames) return STATUS_CONST(INVALID_ARGUMENT);
  if (capacity > SIZE_MAX / stream->channels / sizeof(float))
    return STATUS_CONST(ARITHMETIC_OVERFLOW);
  while (done < capacity && stream->presented < stream->frameCount) {
    uint64_t rawEnd = stream->frameCount == UINT64_MAX ? UINT64_MAX :
        stream->rawFirst + stream->frameCount;
    size_t n;
    if (stream->stagedCursor == stream->stagedFrames) {
      AUDIO_INS(TMP3ReaderStream_Remember)(stream);
      AUDIO_CODEC_TYPE(TMP3FrameInfo) header = {0};
      unsigned char prefix[4];
      size_t decoded = 0;
      if (!stream->sequential && stream->nextByte >= stream->lastByte) {
        status = STATUS_CONST(INCOMPLETE_SEQUENCE);
        break;
      }
      if (stream->sequential) {
        status = AUDIO_INS(MP3_SequentialFill)(reader, stream->encoded, 4,
            &stream->encodedFilled);
        if (status == STATUS_CONST(NOT_FOUND)) {
          if (stream->lengthKnown && stream->presented < stream->frameCount) {
            status=STATUS_CONST(INCOMPLETE_SEQUENCE); break;
          }
          stream->ended = stream->lengthKnown = true;
          stream->frameCount = stream->presented;
          status = STATUS_CONST(SUCCESS);
          break;
        }
        if (status != STATUS_CONST(SUCCESS)) break;
        memcpy(prefix, stream->encoded, 4);
      } else status = AUDIO_INS(MP3_ReaderHeader)(
          reader, stream->nextByte, prefix);
      /* The complete encoded span was validated during OpenReader.
       * A callback reporting EOF inside it means the source was
       * truncated or changed, not that playback ended normally. */
      if (status == STATUS_CONST(NOT_FOUND))
        status = STATUS_CONST(INCOMPLETE_SEQUENCE);
      if (status != STATUS_CONST(SUCCESS)) break;
      if (stream->sequential && !memcmp(prefix,"TAG",3)) {
        size_t tail=stream->encodedFilled, got=0;
        unsigned char extra;
        status=AUDIO_INS(MP3_SequentialFill)(reader,stream->encoded,128,&tail);
        stream->encodedFilled=tail;
        if (status!=STATUS_CONST(SUCCESS)) break;
        status=reader->read(reader->userData,&extra,1,&got);
        if (status!=STATUS_CONST(SUCCESS)) break;
        if (got) { status=STATUS_CONST(INVALID_SEQUENCE); break; }
        if (stream->lengthKnown && stream->presented < stream->frameCount) {
          status=STATUS_CONST(INCOMPLETE_SEQUENCE); break;
        }
        stream->ended=stream->lengthKnown=true;
        stream->frameCount=stream->presented;
        break;
      }
      status = AUDIO_CODEC_FUNC(MP3_ParseHeader)(
          prefix, sizeof prefix, &header);
      if(status==STATUS_CONST(SUCCESS) && (header.frameBytes>sizeof stream->encoded ||
          (!stream->sequential && header.frameBytes>stream->lastByte-stream->nextByte))) {
        status=STATUS_CONST(INCOMPLETE_SEQUENCE); break;
      }
      if(status!=STATUS_CONST(SUCCESS) || header.sampleRate!=stream->rate ||
          header.channels!=stream->channels || header.samplesPerFrame!=stream->samplesPerFrame) {
#if AUDIO_CODEC_MP3_RECOVER_ERRORS
        if(stream->sequential) {
          if(!stream->pendingResyncBytes) {
            memset(&stream->decoder,0,sizeof stream->decoder);
            stream->lengthKnown=false; stream->rawFirst=0; stream->frameCount=UINT64_MAX;
          }
          for(;;) {
            if(stream->pendingResyncBytes>=AUDIO_CODEC_MP3_MAX_RESYNC_BYTES) {
              status=STATUS_CONST(INVALID_SEQUENCE); break;
            }
            memmove(stream->encoded,stream->encoded+1,--stream->encodedFilled);
            ++stream->pendingResyncBytes; ++stream->droppedBytes;
            status=AUDIO_INS(MP3_SequentialFill)(reader,stream->encoded,4,&stream->encodedFilled);
            if(status!=STATUS_CONST(SUCCESS)) break;
            status=AUDIO_CODEC_FUNC(MP3_ParseHeader)(stream->encoded,4,&header);
            if(status==STATUS_CONST(SUCCESS) && header.sampleRate==stream->rate &&
                header.channels==stream->channels && header.samplesPerFrame==stream->samplesPerFrame) {
              stream->pendingResyncBytes=0; break;
            }
          }
          if(status!=STATUS_CONST(SUCCESS)) break;
        } else {
          size_t next;
          status=AUDIO_INS(MP3_ResyncReader)(reader,stream->nextByte,stream->rate,
              stream->channels,stream->samplesPerFrame,&next,prefix,&header);
          if(status!=STATUS_CONST(SUCCESS)) break;
          stream->droppedBytes+=next-stream->nextByte; stream->nextByte=next;
          memset(&stream->decoder,0,sizeof stream->decoder);
        }
#else
        if(status==STATUS_CONST(SUCCESS)) status=STATUS_CONST(INVALID_SEQUENCE);
        break;
#endif
      }
      if(stream->sequential && !stream->lengthKnown) rawEnd=UINT64_MAX;
      if(stream->sequential) stream->pendingResyncBytes=0;
      if(header.frameBytes>sizeof stream->encoded ||
          (!stream->sequential && header.frameBytes>stream->lastByte-stream->nextByte)) {
        status=STATUS_CONST(INCOMPLETE_SEQUENCE); break;
      }
      if (stream->sequential) {
        status = AUDIO_INS(MP3_SequentialFill)(reader, stream->encoded,
            header.frameBytes, &stream->encodedFilled);
      } else {
        status = reader->seek(reader->userData, (int64_t)stream->nextByte,
                              AUDIO_CODEC_CONST(SEEK_START));
        if (status == STATUS_CONST(SUCCESS))
          status = AUDIO_INS(WAV_ReadExact)(reader, stream->encoded, header.frameBytes);
      }
      if (status != STATUS_CONST(SUCCESS)) break;
      status = AUDIO_CODEC_FUNC(MP3_ExperimentFrame)(
          &stream->decoder, stream->encoded, header.frameBytes,
          stream->pcm, 1152, &decoded, &header);
#if AUDIO_CODEC_MP3_RECOVER_ERRORS
      if(status==STATUS_CONST(INVALID_SEQUENCE) ||
          (status==STATUS_CONST(INCOMPLETE_SEQUENCE) &&
           stream->rawDecoded+stream->samplesPerFrame>stream->rawFirst)) {
        if(status==STATUS_CONST(INVALID_SEQUENCE)) memset(&stream->decoder,0,sizeof stream->decoder);
        memset(stream->pcm,0,stream->samplesPerFrame*stream->channels*sizeof(float));
        decoded=stream->samplesPerFrame; stream->concealedFrames+=decoded;
        status=STATUS_CONST(SUCCESS);
      }
#endif
      if (status != STATUS_CONST(SUCCESS) &&
          status != STATUS_CONST(INCOMPLETE_SEQUENCE)) break;
      if (status == STATUS_CONST(SUCCESS) &&
          decoded != stream->samplesPerFrame) {
        status = STATUS_CONST(INVALID_SEQUENCE);
        break;
      }
      stream->stagedFirst = stream->rawDecoded;
      if (stream->rawDecoded > UINT64_MAX-stream->samplesPerFrame) {
        status=STATUS_CONST(ARITHMETIC_OVERFLOW); break;
      }
      stream->rawDecoded += stream->samplesPerFrame;
      if (!stream->sequential) stream->nextByte += header.frameBytes;
      stream->encodedFilled = 0;
      if (status == STATUS_CONST(INCOMPLETE_SEQUENCE)) {
        if (stream->rawDecoded > stream->rawFirst) break;
        stream->stagedFrames = stream->stagedCursor = 0;
        status = STATUS_CONST(SUCCESS);
        continue;
      }
      stream->stagedFrames = decoded;
      stream->stagedCursor = 0;
      if (stream->stagedFirst < stream->rawFirst) {
        uint64_t omit = stream->rawFirst - stream->stagedFirst;
        stream->stagedCursor = omit < decoded ? (size_t)omit : decoded;
      }
    }
    if (stream->stagedCursor == stream->stagedFrames) continue;
    if (stream->stagedFirst + stream->stagedCursor >= rawEnd) break;
    n = stream->stagedFrames - stream->stagedCursor;
    if (n > capacity - done) n = capacity - done;
    if ((uint64_t)n > rawEnd -
        (stream->stagedFirst + stream->stagedCursor))
      n = (size_t)(rawEnd - (stream->stagedFirst + stream->stagedCursor));
    if (!n) break;
    if (samples) memcpy(samples + done * stream->channels,
        stream->pcm + stream->stagedCursor * stream->channels,
        n * stream->channels * sizeof(float));
    stream->stagedCursor += n;
    stream->presented += n;
    done += n;
  }
  *outFrames = done;
  return status;
}

static inline OPSTATUS AUDIO_INS(MP3_ReaderSeek)(
    AUDIO_INS(Decoder) *decoder, uint64_t frameIndex) {
  AUDIO_INS(TMP3ReaderStream) *current, *scratch;
  uint64_t remaining;
  if (!decoder || !decoder->mp3Stream) return STATUS_CONST(INVALID_ARGUMENT);
  current = decoder->mp3Stream;
  if (frameIndex > current->frameCount) return STATUS_CONST(OUT_OF_RANGE);
  if (frameIndex == current->presented) return STATUS_CONST(SUCCESS);
  /* In-frame seeks can adjust the staged PCM cursor without touching
   * the Reader callbacks or restarting the synthesis filterbank. */
  if (current->stagedFrames) {
    uint64_t rawTarget = current->rawFirst + frameIndex;
    uint64_t stagedEnd = current->stagedFirst + current->stagedFrames;
    if (rawTarget >= current->stagedFirst && rawTarget <= stagedEnd) {
      current->stagedCursor = (size_t)(rawTarget - current->stagedFirst);
      current->presented = frameIndex;
      return STATUS_CONST(SUCCESS);
    }
  }
  if (current->sequential) {
    if (frameIndex < current->presented) return STATUS_CONST(NOT_SUPPORTED);
    while (current->presented < frameIndex) {
      size_t got = 0;
      uint64_t left = frameIndex - current->presented;
      OPSTATUS status = AUDIO_INS(MP3_ReaderConsume)(&decoder->reader, current,
          NULL, left > 1152 ? 1152 : (size_t)left, &got);
      if (status != STATUS_CONST(SUCCESS)) return status;
      if (!got) return STATUS_CONST(OUT_OF_RANGE);
    }
    return STATUS_CONST(SUCCESS);
  }
  scratch = (AUDIO_INS(TMP3ReaderStream) *)malloc(sizeof *scratch);
  if (!scratch) return STATUS_CONST(OUT_OF_MEMORY);
  *scratch = *current;
  scratch->indexingDisabled = true;
  /* Reuse decoded history for a forward seek. Only an older target
   * outside the current PCM stage requires a full stream replay. */
  if (frameIndex < current->presented) {
    memset(&scratch->decoder, 0, sizeof scratch->decoder);
    scratch->nextByte = scratch->firstByte;
    scratch->rawDecoded = scratch->stagedFirst = scratch->presented = 0;
    scratch->stagedCursor = scratch->stagedFrames = 0;
    scratch->droppedBytes=scratch->concealedFrames=0;
  }
  {
    AUDIO_INS(TMP3ReaderStreamCheckpoint) *p, *best = NULL;
    for (p = current->checkpoints; p; p = p->next)
      if (p->state.presented <= frameIndex &&
          p->state.presented > scratch->presented &&
          (!best || p->state.presented > best->state.presented)) best = p;
    if (best) { *scratch = best->state; scratch->indexingDisabled = true; }
  }
  {
    uint64_t target=(current->rawFirst+frameIndex)/current->samplesPerFrame;
    const AUDIO_INS(TMP3SeekPoint) *point=target>514 ?
        AUDIO_INS(MP3_IndexBefore)(&current->seekIndex,target-514) : NULL;
    if(point && point->frame*current->samplesPerFrame>scratch->rawDecoded) {
      uint64_t frame=point->frame; size_t at=point->byte;
      memset(&scratch->decoder,0,sizeof scratch->decoder);
      while(frame<target) {
        AUDIO_CODEC_TYPE(TMP3FrameInfo) header={0};
        OPSTATUS status=AUDIO_INS(MP3_ReaderHeader)(&decoder->reader,at,scratch->encoded);
        if(status==STATUS_CONST(SUCCESS)) status=AUDIO_CODEC_FUNC(MP3_ParseHeader)(scratch->encoded,4,&header);
        if(status==STATUS_CONST(SUCCESS) && (header.frameBytes>sizeof scratch->encoded ||
            header.frameBytes>current->lastByte-at || header.sampleRate!=current->rate ||
            header.channels!=current->channels || header.samplesPerFrame!=current->samplesPerFrame))
          status=STATUS_CONST(INVALID_SEQUENCE);
        if(status==STATUS_CONST(SUCCESS)) status=AUDIO_INS(WAV_ReadExact)(
            &decoder->reader,scratch->encoded+4,header.frameBytes-4);
        if(status==STATUS_CONST(SUCCESS)) status=AUDIO_INS(MP3_SeekPrerollFrame)(
            &scratch->decoder,scratch->encoded,&header,frame+2>=target,scratch->pcm);
        if(status!=STATUS_CONST(SUCCESS)) { free(scratch); return status; }
        at+=header.frameBytes; ++frame;
      }
      scratch->nextByte=at; scratch->rawDecoded=target*current->samplesPerFrame;
      scratch->stagedFirst=scratch->rawDecoded; scratch->encodedFilled=0;
      scratch->stagedCursor=scratch->stagedFrames=0;
      scratch->presented=scratch->rawDecoded>scratch->rawFirst ? scratch->rawDecoded-scratch->rawFirst : 0;
    }
  }
  remaining = frameIndex - scratch->presented;
  while (remaining) {
    size_t n = remaining > 1152 ? 1152 : (size_t)remaining, got = 0;
    OPSTATUS status = AUDIO_INS(MP3_ReaderConsume)(
        &decoder->reader, scratch, NULL, n, &got);
    if (status != STATUS_CONST(SUCCESS) || got != n) {
      free(scratch);
      return status == STATUS_CONST(SUCCESS) ?
          STATUS_CONST(INCOMPLETE_SEQUENCE) : status;
    }
    remaining -= got;
  }
  scratch->seekIndex = current->seekIndex;
  scratch->checkpoints = current->checkpoints;
  scratch->checkpointCount = current->checkpointCount;
  scratch->indexingDisabled = false;
  *current = *scratch;
  free(scratch);
  return STATUS_CONST(SUCCESS);
}
