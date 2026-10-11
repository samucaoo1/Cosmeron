#pragma once
#include "MP3HuffmanTables.inc"

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
