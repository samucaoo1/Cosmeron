#pragma once
/* Lightweight byte offsets are collected during the existing header scan.
 * When full, retain every other entry; storage remains bounded to 1 MiB. */
typedef struct AUDIO_INS(TMP3SeekPoint) { uint64_t frame; size_t byte; } AUDIO_INS(TMP3SeekPoint);
typedef struct AUDIO_INS(TMP3SeekIndex) {
  AUDIO_INS(TMP3SeekPoint) *points;
  size_t count, capacity;
  uint64_t stride;
  bool disabled;
} AUDIO_INS(TMP3SeekIndex);
static inline void AUDIO_INS(MP3_IndexRecord)(AUDIO_INS(TMP3SeekIndex) *index,
    uint64_t frame, size_t byte) {
  size_t i;
  if (!index->stride) index->stride=64;
  if (index->disabled || frame%index->stride) return;
  if (index->count==65536) {
    for(i=0;i<index->count/2;++i) index->points[i]=index->points[i*2];
    index->count/=2; index->stride*=2;
    if (frame%index->stride) return;
  }
  if (index->count==index->capacity) {
    size_t capacity=index->capacity ? index->capacity*2 : 64;
    AUDIO_INS(TMP3SeekPoint) *p=(AUDIO_INS(TMP3SeekPoint) *)realloc(index->points,capacity*sizeof *p);
    if (!p) { index->disabled=true; return; }
    index->points=p; index->capacity=capacity;
  }
  index->points[index->count].frame=frame;
  index->points[index->count++].byte=byte;
}
static inline const AUDIO_INS(TMP3SeekPoint) *AUDIO_INS(MP3_IndexBefore)(
    const AUDIO_INS(TMP3SeekIndex) *index,uint64_t frame) {
  size_t lo=0,hi=index->count;
  while(lo<hi) { size_t mid=lo+(hi-lo)/2;
    if(index->points[mid].frame<=frame) lo=mid+1; else hi=mid;
  }
  return lo ? &index->points[lo-1] : NULL;
}
/* At least 512 transmitted payload bytes restore the 511-byte reservoir.
 * Two fully decoded predecessor frames then restore hybrid overlap and
 * the synthesis delay line, including MPEG-2/2.5's 18-slot frames. */
static inline OPSTATUS AUDIO_INS(MP3_SeekPrerollFrame)(
    AUDIO_CODEC_TYPE(TMP3Experimental) *state,const unsigned char *frame,
    const AUDIO_CODEC_TYPE(TMP3FrameInfo) *header,bool synthesize,float *pcm) {
  const unsigned char *mainData; size_t mainBytes,decoded=0;
  AUDIO_CODEC_TYPE(TMP3SideInfo) side;
  AUDIO_CODEC_TYPE(TMP3FrameInfo) result;
  OPSTATUS status;
  if (synthesize) return AUDIO_CODEC_FUNC(MP3_ExperimentFrame)(state,frame,
      header->frameBytes,pcm,1152,&decoded,&result);
  status=AUDIO_CODEC_FUNC(MP3_CheckCRC)(frame,header->frameBytes);
  if(status!=STATUS_CONST(SUCCESS)) return status;
  status=AUDIO_CODEC_FUNC(MP3_ReservoirFeed)(&state->reservoir,frame,
      header->frameBytes,&mainData,&mainBytes,&side);
  if(status==STATUS_CONST(INCOMPLETE_SEQUENCE)) status=STATUS_CONST(SUCCESS);
  if(status==STATUS_CONST(SUCCESS) && !side.mainDataBytes)
    return STATUS_CONST(INVALID_SEQUENCE);
  return status;
}
