#pragma once
#include "FLACMD5.inc"
#include "FLACStream.inc"

typedef struct AUDIO_INS(TFLACBits) {
  const unsigned char *bytes;
  size_t size;
  size_t bit;
} AUDIO_INS(TFLACBits);

static inline bool AUDIO_INS(FLAC_ReadBits)(
    AUDIO_INS(TFLACBits) *s, unsigned int count, uint64_t *result) {
  uint64_t value = 0;
  unsigned int i;
  if (count > 63 || s->bit > s->size * 8 ||
      count > s->size * 8 - s->bit) return false;
  for (i = 0; i < count; ++i) {
    size_t at = s->bit++;
    value = (value << 1) | ((s->bytes[at >> 3] >> (7 - (at & 7))) & 1U);
  }
  *result = value;
  return true;
}
static inline bool AUDIO_INS(FLAC_Signed)(
    AUDIO_INS(TFLACBits) *s, unsigned int width, int64_t *result) {
  uint64_t raw;
  if (!width || width > 40 || !AUDIO_INS(FLAC_ReadBits)(s, width, &raw))
    return false;
  *result = raw & (UINT64_C(1) << (width - 1)) ?
      (int64_t)raw - (int64_t)(UINT64_C(1) << width) : (int64_t)raw;
  return true;
}
/* FLAC bitstreams are untrusted. LPC coefficients and decoded residuals
 * can individually fit int64_t while their arithmetic overflows it.
 * Guard each operation before evaluating signed expressions (portable C11). */
static inline bool AUDIO_INS(FLAC_AddI64)(
    int64_t a, int64_t b, int64_t *result) {
  if ((b > 0 && a > INT64_MAX - b) ||
      (b < 0 && a < INT64_MIN - b)) return false;
  *result = a + b;
  return true;
}
static inline bool AUDIO_INS(FLAC_SubI64)(
    int64_t a, int64_t b, int64_t *result) {
  if ((b > 0 && a < INT64_MIN + b) ||
      (b < 0 && a > INT64_MAX + b)) return false;
  *result = a - b;
  return true;
}
static inline bool AUDIO_INS(FLAC_MulI64)(
    int64_t a, int64_t b, int64_t *result) {
  if (a && b) {
    if (a > 0) {
      if ((b > 0 && a > INT64_MAX / b) ||
          (b < 0 && b < INT64_MIN / a)) return false;
    } else {
      if ((b > 0 && a < INT64_MIN / b) ||
          (b < 0 && a < INT64_MAX / b)) return false;
    }
  }
  *result = a * b;
  return true;
}

static inline unsigned int AUDIO_INS(FLAC_CRC8)(
    const unsigned char *data, size_t size) {
  unsigned int crc = 0;
  size_t i;
  for (i = 0; i < size; ++i) {
    unsigned int j;
    crc ^= data[i];
    for (j = 0; j < 8; ++j)
      crc = (crc << 1) ^ ((crc & 0x80U) ? 0x07U : 0U);
    crc &= 0xffU;
  }
  return crc;
}
static inline unsigned int AUDIO_INS(FLAC_CRC16)(
    const unsigned char *data, size_t size) {
  unsigned int crc = 0;
  size_t i;
  for (i = 0; i < size; ++i) {
    unsigned int j;
    crc ^= (unsigned int)data[i] << 8;
    for (j = 0; j < 8; ++j)
      crc = ((crc << 1) ^ ((crc & 0x8000U) ? 0x8005U : 0U)) & 0xffffU;
  }
  return crc;
}
static inline bool AUDIO_INS(FLAC_Unary)(
    AUDIO_INS(TFLACBits) *bits, unsigned int maximum, uint64_t *result) {
  uint64_t b, count = 0;
  do {
    if (count > maximum || !AUDIO_INS(FLAC_ReadBits)(bits, 1, &b))
      return false;
    if (b) break;
    ++count;
  } while (true);
  *result = count;
  return true;
}
static inline bool AUDIO_INS(FLAC_UTF8Number)(
    AUDIO_INS(TFLACBits) *s, uint64_t *number) {
  uint64_t first, value, next;
  unsigned int i, continuation;
  if (!AUDIO_INS(FLAC_ReadBits)(s, 8, &first)) return false;
  if (!(first & 0x80)) { *number = first; return true; }
  continuation = 0;
  for (i = 7; i; --i) {
    if (!(first & (UINT64_C(1) << i))) break;
    ++continuation;
  }
  if (continuation < 2 || continuation > 7) return false;
  value = first & ((UINT64_C(1) << (7 - continuation)) - 1);
  for (i = 1; i < continuation; ++i) {
    if (!AUDIO_INS(FLAC_ReadBits)(s, 8, &next) ||
        (next & 0xc0U) != 0x80U) return false;
    value = (value << 6) | (next & 63);
  }
  *number = value;
  return true;
}
static inline bool AUDIO_INS(FLAC_Residual)(
    AUDIO_INS(TFLACBits) *s, int64_t *out, size_t frames,
    unsigned int predictor) {
  uint64_t method, order;
  size_t partitions, partSize, part;
  if (!AUDIO_INS(FLAC_ReadBits)(s, 2, &method) || method > 1 ||
      !AUDIO_INS(FLAC_ReadBits)(s, 4, &order)) return false;
  partitions = (size_t)1 << (unsigned int)order;
  if (frames % partitions) return false;
  partSize = frames / partitions;
  if (partSize < predictor) return false;
  for (part = 0; part < partitions; ++part) {
    uint64_t rice;
    size_t i = part * partSize, end = i + partSize;
    unsigned int bits = method ? 5 : 4;
    if (!AUDIO_INS(FLAC_ReadBits)(s, bits, &rice)) return false;
    if (part == 0) i += predictor;
    if (rice == ((UINT64_C(1) << bits) - 1)) {
      uint64_t rawWidth;
      if (!AUDIO_INS(FLAC_ReadBits)(s, 5, &rawWidth)) return false;
      for (; i < end; ++i) {
        if (!rawWidth) out[i] = 0;
        else if (!AUDIO_INS(FLAC_Signed)(s, (unsigned int)rawWidth, &out[i]))
          return false;
      }
    } else {
      for (; i < end; ++i) {
        uint64_t quotient, remainder = 0, unsignedValue;
        if (!AUDIO_INS(FLAC_Unary)(s, UINT32_C(0x1000000), &quotient) ||
            !AUDIO_INS(FLAC_ReadBits)(s, (unsigned int)rice, &remainder))
          return false;
        if (quotient > (UINT64_C(1) << 38) >> (unsigned int)rice)
          return false;
        unsignedValue = (quotient << rice) | remainder;
        out[i] = (int64_t)(unsignedValue >> 1);
        if (unsignedValue & 1) out[i] = -out[i] - 1;
      }
    }
  }
  return true;
}
static inline bool AUDIO_INS(FLAC_Subframe)(
    AUDIO_INS(TFLACBits) *s, int64_t *samples,
    size_t count, unsigned int sampleBits) {
  uint64_t padding, kind, wastedFlag, unary;
  unsigned int shift = 0, order = 0, i;
  int64_t value;
  if (!AUDIO_INS(FLAC_ReadBits)(s, 1, &padding) || padding ||
      !AUDIO_INS(FLAC_ReadBits)(s, 6, &kind) ||
      !AUDIO_INS(FLAC_ReadBits)(s, 1, &wastedFlag)) return false;
  if (wastedFlag) {
    if (!AUDIO_INS(FLAC_Unary)(s, 32, &unary)) return false;
    shift = (unsigned int)unary + 1;
    if (shift >= sampleBits) return false;
  }
  sampleBits -= shift;
  if (kind == 0) {
    if (!AUDIO_INS(FLAC_Signed)(s, sampleBits, &value)) return false;
    for (i = 0; i < count; ++i) samples[i] = value;
  } else if (kind == 1) {
    for (i = 0; i < count; ++i)
      if (!AUDIO_INS(FLAC_Signed)(s, sampleBits, &samples[i]))
        return false;
  } else if ((kind & 0x38U) == 8U || (kind & 0x20U)) {
    bool lpc = (kind & 0x20U) != 0;
    uint64_t precision = 0, rawShift = 0;
    int64_t coeff[32] = {0};
    order = lpc ? (unsigned int)(kind & 31U) + 1 :
        (unsigned int)(kind & 7U);
    if ((!lpc && order > 4) || order > count) return false;
    for (i = 0; i < order; ++i)
      if (!AUDIO_INS(FLAC_Signed)(s, sampleBits, &samples[i]))
        return false;
    if (lpc) {
      if (!AUDIO_INS(FLAC_ReadBits)(s, 4, &precision) || precision == 15 ||
          !AUDIO_INS(FLAC_ReadBits)(s, 5, &rawShift))
        return false;
      ++precision;
      for (i = 0; i < order; ++i)
        if (!AUDIO_INS(FLAC_Signed)(s, (unsigned int)precision, &coeff[i]))
          return false;
    }
    if (!AUDIO_INS(FLAC_Residual)(s, samples, count, order)) return false;
    for (i = order; i < count; ++i) {
      int64_t prediction = 0;
      unsigned int j;
      if (lpc) {
        int64_t shiftValue = rawShift & 16U ? (int64_t)rawShift - 32 :
            (int64_t)rawShift;
        for (j = 0; j < order; ++j) {
          int64_t term;
          if (!AUDIO_INS(FLAC_MulI64)(
                  coeff[j], samples[i - j - 1], &term) ||
              !AUDIO_INS(FLAC_AddI64)(prediction, term, &prediction))
            return false;
        }
        if (shiftValue >= 0) {
          /* Right shift of negative signed values is implementation-
           * defined in C11; FLAC requires floor division. */
          int64_t divisor = INT64_C(1) << (unsigned int)shiftValue;
          int64_t quotient = prediction / divisor;
          if (prediction < 0 && prediction % divisor) --quotient;
          prediction = quotient;
        } else if (!AUDIO_INS(FLAC_MulI64)(
                       prediction, INT64_C(1) << (unsigned int)-shiftValue,
                       &prediction)) return false;
      } else {
        int64_t term;
        switch (order) {
        case 0: prediction = 0; break;
        case 1: prediction = samples[i - 1]; break;
        case 2:
          if (!AUDIO_INS(FLAC_MulI64)(2, samples[i - 1], &prediction) ||
              !AUDIO_INS(FLAC_SubI64)(prediction, samples[i - 2], &prediction))
            return false;
          break;
        case 3:
          if (!AUDIO_INS(FLAC_MulI64)(3, samples[i - 1], &prediction) ||
              !AUDIO_INS(FLAC_MulI64)(3, samples[i - 2], &term) ||
              !AUDIO_INS(FLAC_SubI64)(prediction, term, &prediction) ||
              !AUDIO_INS(FLAC_AddI64)(prediction, samples[i - 3], &prediction))
            return false;
          break;
        default:
          if (!AUDIO_INS(FLAC_MulI64)(4, samples[i - 1], &prediction) ||
              !AUDIO_INS(FLAC_MulI64)(6, samples[i - 2], &term) ||
              !AUDIO_INS(FLAC_SubI64)(prediction, term, &prediction) ||
              !AUDIO_INS(FLAC_MulI64)(4, samples[i - 3], &term) ||
              !AUDIO_INS(FLAC_AddI64)(prediction, term, &prediction) ||
              !AUDIO_INS(FLAC_SubI64)(prediction, samples[i - 4], &prediction))
            return false;
        }
      }
      if (!AUDIO_INS(FLAC_AddI64)(samples[i], prediction, &samples[i]))
        return false;
    }
  } else return false;
  if (shift) {
    for (i = 0; i < count; ++i) {
      if (!AUDIO_INS(FLAC_MulI64)(
              samples[i], INT64_C(1) << shift, &samples[i])) return false;
    }
  }
  return true;
}

/* Fully checks the frame header/subframes and CRCs before exposing PCM. */
static inline OPSTATUS AUDIO_INS(FLAC_DecodeFrameDigest)(
    const unsigned char *source, size_t size,
    const AUDIO_INS(TFLACStreamInfo) *stream, size_t offset,
    float *destination, size_t capacityFrames,
    size_t *outFrames, size_t *outNext, AUDIO_INS(TFLACMD5) *digest) {
  AUDIO_INS(TFLACBits) bits = {source, size, 0};
  uint64_t sync, blocking, blockCode, rateCode, assignment, depthCode, reserved;
  uint64_t number, value;
  uint32_t channels, sampleBits, sampleRate;
  size_t headerEnd, count, i, frameBytes, frameStart = offset;
  int64_t *pcm = NULL;
  AUDIO_INS(TFLACMD5) digestWork;
  if (digest) digestWork = *digest;
  unsigned int ch;
  OPSTATUS status = STATUS_CONST(INVALID_SEQUENCE);
  if (!source || !stream || !destination || !outFrames || !outNext ||
      size > SIZE_MAX / 8 || offset > size) return STATUS_CONST(INVALID_ARGUMENT);
  bits.bit = offset * 8;
  if (!AUDIO_INS(FLAC_ReadBits)(&bits, 14, &sync) || sync != 0x3ffe ||
      !AUDIO_INS(FLAC_ReadBits)(&bits, 1, &reserved) || reserved ||
      !AUDIO_INS(FLAC_ReadBits)(&bits, 1, &blocking) ||
      !AUDIO_INS(FLAC_ReadBits)(&bits, 4, &blockCode) || !blockCode ||
      !AUDIO_INS(FLAC_ReadBits)(&bits, 4, &rateCode) || rateCode == 15 ||
      !AUDIO_INS(FLAC_ReadBits)(&bits, 4, &assignment) || assignment > 10 ||
      !AUDIO_INS(FLAC_ReadBits)(&bits, 3, &depthCode) ||
      depthCode == 3 ||
      !AUDIO_INS(FLAC_ReadBits)(&bits, 1, &reserved) || reserved ||
      !AUDIO_INS(FLAC_UTF8Number)(&bits, &number))
    return STATUS_CONST(INVALID_SEQUENCE);
  (void)blocking;
  (void)number;
  if (blockCode == 1) count = 192;
  else if (blockCode < 6) count = (size_t)576 << (unsigned int)(blockCode - 2);
  else if (blockCode == 6 || blockCode == 7) {
    if (!AUDIO_INS(FLAC_ReadBits)(&bits, blockCode == 6 ? 8 : 16, &value))
      return STATUS_CONST(INCOMPLETE_SEQUENCE);
    count = (size_t)value + 1;
  } else count = (size_t)256 << (unsigned int)(blockCode - 8);
  if (rateCode == 0) sampleRate = stream->sampleRate;
  else if (rateCode == 12 || rateCode == 13 || rateCode == 14) {
    if (!AUDIO_INS(FLAC_ReadBits)(&bits, rateCode == 12 ? 8 : 16, &value))
      return STATUS_CONST(INCOMPLETE_SEQUENCE);
    sampleRate = (uint32_t)value * (rateCode == 12 ? 1000U : rateCode == 14 ? 10U : 1U);
  } else {
    static const uint32_t rates[] = {0,88200,176400,192000,8000,16000,22050,24000,
        32000,44100,48000,96000};
    sampleRate = rates[rateCode];
  }
  channels = assignment <= 7 ? (uint32_t)assignment + 1 : 2;
  sampleBits = depthCode == 0 ? stream->bitsPerSample :
      depthCode == 1 ? 8 : depthCode == 2 ? 12 :
      depthCode == 4 ? 16 : depthCode == 5 ? 20 : depthCode == 6 ? 24 : 32;
  if (channels != stream->channels || sampleRate != stream->sampleRate ||
      sampleBits != stream->bitsPerSample || count > capacityFrames ||
      count > SIZE_MAX / channels / sizeof(int64_t))
    return STATUS_CONST(INVALID_SEQUENCE);
  headerEnd = bits.bit / 8;
  if (!AUDIO_INS(FLAC_ReadBits)(&bits, 8, &value) ||
      value != AUDIO_INS(FLAC_CRC8)(source + frameStart, headerEnd - frameStart))
    return STATUS_CONST(INVALID_SEQUENCE);
  pcm = (int64_t *)malloc(count * channels * sizeof(*pcm));
  if (!pcm) return STATUS_CONST(OUT_OF_MEMORY);
  for (ch = 0; ch < channels; ++ch) {
    unsigned int width = sampleBits + ((assignment == 8 && ch == 1) ||
        (assignment == 9 && ch == 0) || (assignment == 10 && ch == 1));
    if (!AUDIO_INS(FLAC_Subframe)(&bits, pcm + (size_t)ch * count, count, width))
      goto end;
  }
  while (bits.bit & 7U) {
    if (!AUDIO_INS(FLAC_ReadBits)(&bits, 1, &value) || value) goto end;
  }
  frameBytes = bits.bit / 8;
  if (frameBytes < frameStart || size - frameBytes < 2 ||
      !AUDIO_INS(FLAC_ReadBits)(&bits, 16, &value) ||
      value != AUDIO_INS(FLAC_CRC16)(source + frameStart, frameBytes - frameStart))
    goto end;
  for (i = 0; i < count; ++i) {
    int64_t left = pcm[i], right = channels == 2 ? pcm[count + i] : 0;
    if (assignment == 8) {
      if (!AUDIO_INS(FLAC_SubI64)(left, right, &right)) goto end;
    } else if (assignment == 9) {
      if (!AUDIO_INS(FLAC_AddI64)(left, right, &left)) goto end;
    } else if (assignment == 10) {
      int64_t mid, restoredLeft, restoredRight;
      if (!AUDIO_INS(FLAC_MulI64)(left, 2, &mid) ||
          !AUDIO_INS(FLAC_AddI64)(mid, right & 1, &mid) ||
          !AUDIO_INS(FLAC_AddI64)(mid, right, &restoredLeft) ||
          !AUDIO_INS(FLAC_SubI64)(mid, right, &restoredRight))
        goto end;
      left = restoredLeft / 2;
      right = restoredRight / 2;
    }
    for (ch = 0; ch < channels; ++ch) {
      int64_t sample = ch == 0 ? left : right;
      if (sample < -(INT64_C(1) << (sampleBits - 1)) ||
          sample >= (INT64_C(1) << (sampleBits - 1))) goto end;
      if (digest) {
        unsigned char encoded[4];
        unsigned int j, width = (sampleBits + 7U) / 8U;
        for (j = 0; j < width; ++j)
          encoded[j] = (unsigned char)((uint64_t)sample >> (8U*j));
        AUDIO_INS(FLAC_MD5Update)(&digestWork, encoded, width);
      }
      destination[i * channels + ch] =
          (float)((double)sample / (double)(INT64_C(1) << (sampleBits - 1)));
    }
  }
  if (digest) *digest = digestWork;
  *outFrames = count;
  *outNext = bits.bit / 8;
  status = STATUS_CONST(SUCCESS);
end:
  free(pcm);
  return status;
}

static inline OPSTATUS AUDIO_INS(FLAC_DecodeFrame)(
    const unsigned char *source, size_t size,
    const AUDIO_INS(TFLACStreamInfo) *stream, size_t offset,
    float *destination, size_t capacityFrames, size_t *outFrames, size_t *outNext) {
  return AUDIO_INS(FLAC_DecodeFrameDigest)(source, size, stream, offset,
      destination, capacityFrames, outFrames, outNext, NULL);
}
