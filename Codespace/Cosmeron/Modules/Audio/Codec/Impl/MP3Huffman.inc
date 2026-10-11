#pragma once
#include "MP3HuffmanTables.inc"
#include "MP3HuffmanBands.inc"

/* Consume only bits within [*ioBit, endBit). A partial codeword leaves
 * caller's bit cursor untouched, so streaming input can be retried. */
static inline OPSTATUS AUDIO_INS(MP3_MatchHuffman)(
    const unsigned char *bytes, size_t byteCount,
    size_t *ioBit, size_t endBit,
    const uint16_t *codes, const uint8_t *lengths,
    size_t symbols, unsigned int maxLength, unsigned int *outIndex) {
  size_t index, cursor;
  uint32_t code = 0;
  unsigned int length;
  if (!bytes || !ioBit || !outIndex || !codes || !lengths ||
      byteCount > SIZE_MAX / 8 || endBit > byteCount * 8 ||
      *ioBit > endBit || maxLength > 24)
    return STATUS_CONST(INVALID_ARGUMENT);
  cursor = *ioBit;
  for (length = 1; length <= maxLength; ++length) {
    if (cursor >= endBit) return STATUS_CONST(INCOMPLETE_SEQUENCE);
    code = (code << 1) |
        ((bytes[cursor / 8] >> (7U - (cursor & 7U))) & 1U);
    ++cursor;
    for (index = 0; index < symbols; ++index) {
      if (lengths[index] == length && codes[index] == code) {
        *outIndex = (unsigned int)index;
        *ioBit = cursor;
        return STATUS_CONST(SUCCESS);
      }
    }
  }
  return STATUS_CONST(INVALID_SEQUENCE);
}

static inline bool AUDIO_INS(MP3_GetBounded)(
    AUDIO_INS(TMP3Bits) *bits, size_t endBit,
    unsigned int width, unsigned int *value) {
  return bits->cursor <= endBit && width <= endBit - bits->cursor &&
      AUDIO_INS(MP3_ReadBits)(bits, width, value);
}

/* Standard MP3 pair: Huffman (x,y), optional linbits for escape value 15,
 * then sign bit for each nonzero component. No vendor dependencies. */
AUDIO_CODEC_MP3_HUFFMAN_PAIR_PROTOTYPE {
  AUDIO_INS(TMP3Book) book;
  AUDIO_INS(TMP3Bits) bits;
  size_t cursor;
  unsigned int linbits = 0, index = 0, value = 0, x, y;
  OPSTATUS status;
  if (!data || !ioBit || !outX || !outY ||
      size > SIZE_MAX / 8 || endBit > size * 8 ||
      *ioBit > endBit || !AUDIO_INS(MP3_BookFor)(table, &book, &linbits))
    return STATUS_CONST(INVALID_ARGUMENT);
  if (table == 0) {
    *outX = *outY = 0;
    return STATUS_CONST(SUCCESS);
  }
  cursor = *ioBit;
  status = AUDIO_INS(MP3_MatchHuffman)(data, size, &cursor, endBit,
      book.codes, book.lengths, book.symbols, book.maximumLength, &index);
  if (status != STATUS_CONST(SUCCESS)) return status;
  x = index / book.alphabet;
  y = index % book.alphabet;
  bits.data = data;
  bits.bytes = size;
  bits.cursor = cursor;
  if (x == 15 && linbits) {
    if (!AUDIO_INS(MP3_GetBounded)(&bits, endBit, linbits, &value))
      return STATUS_CONST(INCOMPLETE_SEQUENCE);
    x += value;
  }
  if (x) {
    if (!AUDIO_INS(MP3_GetBounded)(&bits, endBit, 1, &value))
      return STATUS_CONST(INCOMPLETE_SEQUENCE);
    if (value) x |= UINT32_C(0x80000000);
  }
  if (y == 15 && linbits) {
    if (!AUDIO_INS(MP3_GetBounded)(&bits, endBit, linbits, &value))
      return STATUS_CONST(INCOMPLETE_SEQUENCE);
    y += value;
  }
  if (y) {
    if (!AUDIO_INS(MP3_GetBounded)(&bits, endBit, 1, &value))
      return STATUS_CONST(INCOMPLETE_SEQUENCE);
    if (value) y |= UINT32_C(0x80000000);
  }
  /* A zero coefficient never carries a sign; signed magnitudes are kept
   * in uint32_t until conversion so table 31 extremes cannot overflow. */
  *outX = (int32_t)(x & UINT32_C(0x7fffffff));
  *outY = (int32_t)(y & UINT32_C(0x7fffffff));
  if (x & UINT32_C(0x80000000)) *outX = -*outX;
  if (y & UINT32_C(0x80000000)) *outY = -*outY;
  *ioBit = bits.cursor;
  return STATUS_CONST(SUCCESS);
}

/* Count1 encodes four 0/1 coefficients (v,w,x,y), not 2 pair symbols. */
AUDIO_CODEC_MP3_HUFFMAN_COUNT1_PROTOTYPE {
  AUDIO_INS(TMP3Bits) bits;
  unsigned int symbol, i, sign;
  size_t cursor;
  OPSTATUS status;
  if (!data || !ioBit || !outQuad || table < 32 || table > 33 ||
      size > SIZE_MAX / 8 || endBit > size * 8 || *ioBit > endBit)
    return STATUS_CONST(INVALID_ARGUMENT);
  cursor = *ioBit;
  status = AUDIO_INS(MP3_MatchHuffman)(data, size, &cursor, endBit,
      AUDIO_INS(MP3QuadCodes0),
      AUDIO_INS(MP3QuadLengths0),
      16, 6, &symbol);
  /* Table 33 has an independent, fixed-width 4-bit assignment. */
  if (table == 33) {
    cursor = *ioBit;
    status = AUDIO_INS(MP3_MatchHuffman)(data, size, &cursor, endBit,
        AUDIO_INS(MP3QuadCodes1), AUDIO_INS(MP3QuadLengths1), 16, 4, &symbol);
  }
  if (status != STATUS_CONST(SUCCESS)) return status;
  bits.data = data;
  bits.bytes = size;
  bits.cursor = cursor;
  for (i = 0; i < 4; ++i) {
    unsigned int component = (symbol >> (3U - i)) & 1U;
    if (component) {
      if (!AUDIO_INS(MP3_GetBounded)(&bits, endBit, 1, &sign))
        return STATUS_CONST(INCOMPLETE_SEQUENCE);
      outQuad[i] = sign ? -1 : 1;
    } else outQuad[i] = 0;
  }
  *ioBit = bits.cursor;
  return STATUS_CONST(SUCCESS);
}

/* Read quantized spectral coefficients of one granule/channel. Region
 * boundaries use MPEG long-window scalefactor bands; switched short blocks
 * use the format-defined 36-line boundary. This is pre-requantization data. */
AUDIO_CODEC_MP3_HUFFMAN_GRANULE_PROTOTYPE {
  const AUDIO_CODEC_TYPE(TMP3Granule) *part;
  int32_t quantized[576] = {0};
  size_t offset = 0, end, cursor, pair, count = 0, region0 = 576, region1 = 576;
  unsigned int rateIndex, i, table;
  OPSTATUS status;
  if (!mainData || !side || !scales || !outValues || !outCount ||
      side->channels < 1 || side->channels > 2 ||
      side->granules < 1 || side->granules > 2 ||
      granule >= side->granules || channel >= side->channels)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (mainDataBytes > SIZE_MAX / 8)
    return STATUS_CONST(OUT_OF_RANGE);
  for (i = 0; i < granule; ++i) {
    unsigned int c;
    for (c = 0; c < side->channels; ++c)
      offset += side->parts[i][c].part23Length;
  }
  for (i = 0; i < channel; ++i)
    offset += side->parts[granule][i].part23Length;
  part = &side->parts[granule][channel];
  if (part->part23Length > mainDataBytes * 8 ||
      offset > mainDataBytes * 8 - part->part23Length)
    return STATUS_CONST(INCOMPLETE_SEQUENCE);
  if (scales->part2Bits > part->part23Length || part->bigValues > 288)
    return STATUS_CONST(INVALID_SEQUENCE);
  end = offset + part->part23Length;
  cursor = offset + scales->part2Bits;
  if (part->windowSwitching && part->blockType == 2) {
    region0 = 36;
    region1 = 576;
  } else {
    unsigned int boundary0 = (unsigned int)part->region0Count + 1U;
    unsigned int boundary1 = boundary0 + part->region1Count + 1U;
    for (rateIndex = 0; rateIndex < 9; ++rateIndex)
      if (AUDIO_INS(MP3_LongRates)[rateIndex] == sampleRate) break;
    if (rateIndex == 9) return STATUS_CONST(NOT_SUPPORTED);
    if (boundary0 > 22 || boundary1 > 22)
      return STATUS_CONST(INVALID_SEQUENCE);
    region0 = AUDIO_INS(MP3_LongBands)[rateIndex][boundary0];
    region1 = AUDIO_INS(MP3_LongBands)[rateIndex][boundary1];
  }
  for (pair = 0; pair < part->bigValues; ++pair) {
    int32_t x, y;
    count = pair * 2;
    table = count < region0 ? part->tableSelect[0] :
        count < region1 ? part->tableSelect[1] : part->tableSelect[2];
    status = AUDIO_CODEC_FUNC(MP3_HuffmanPair)(
        mainData, mainDataBytes, &cursor, end, table, &x, &y);
    if (status != STATUS_CONST(SUCCESS)) return status;
    quantized[count] = x;
    quantized[count + 1] = y;
  }
  count = part->bigValues * 2;
  /* The count1 partition may end in stuffing. Incomplete final quadruples
   * are ignored, preserving the previously decoded pair coefficients. */
  while (count <= 572 && cursor < end) {
    int32_t values[4] = {0};
    size_t trial = cursor;
    status = AUDIO_CODEC_FUNC(MP3_HuffmanCount1)(
        mainData, mainDataBytes, &trial, end,
        32U + part->count1TableSelect, values);
    if (status == STATUS_CONST(INCOMPLETE_SEQUENCE)) break;
    if (status != STATUS_CONST(SUCCESS)) return status;
    if (trial <= cursor) return STATUS_CONST(INVALID_SEQUENCE);
    memcpy(quantized + count, values, sizeof values);
    cursor = trial;
    count += 4;
  }
  memcpy(outValues, quantized, sizeof quantized);
  *outCount = count;
  return STATUS_CONST(SUCCESS);
}
