#include "../../Cosmeron/Modules/Audio/Codec/MP3.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

static void putBits(unsigned char *data, size_t *cursor, unsigned int value,
                    unsigned int bits) {
  unsigned int i;
  for (i = bits; i; --i) {
    size_t at = (*cursor)++;
    if ((value >> (i - 1U)) & 1U)
      data[at / 8] |= (unsigned char)(0x80U >> (at & 7U));
  }
}
static void checkPair(unsigned int table, unsigned int symbol) {
  AUDIO_INS(TMP3Book) book;
  unsigned int linbits = 0, x, y, sx, sy, extra = 0;
  unsigned char data[16] = {0};
  size_t cursor = 3, first = cursor, end, cursorCopy;
  int32_t dx = -999, dy = -999, expectedX, expectedY;
  OPSTATUS status;
  assert(AUDIO_INS(MP3_BookFor)(table, &book, &linbits));
  assert(book.symbols && symbol < book.symbols && book.alphabet);
  x = symbol / book.alphabet; y = symbol % book.alphabet;
  sx = x != 0 && (symbol & 1U); sy = y != 0 && ((symbol >> 1) & 1U);
  putBits(data, &cursor, book.codes[symbol], book.lengths[symbol]);
  if (x == 15 && linbits) {
    extra = ((unsigned int)1 << linbits) - 1;
    putBits(data, &cursor, extra, linbits);
  }
  if (x) putBits(data, &cursor, sx, 1);
  if (y == 15 && linbits) putBits(data, &cursor, extra, linbits);
  if (y) putBits(data, &cursor, sy, 1);
  end = cursor;
  expectedX = (int32_t)(x + (x == 15 ? extra : 0));
  expectedY = (int32_t)(y + (y == 15 ? extra : 0));
  if (sx) expectedX = -expectedX;
  if (sy) expectedY = -expectedY;
  cursorCopy = first;
  status = AUDIO_CODEC_FUNC(MP3_HuffmanPair)(data, sizeof data,
      &cursorCopy, end, table, &dx, &dy);
  assert(status == STATUS_CONST(SUCCESS));
  assert(cursorCopy == end && dx == expectedX && dy == expectedY);
  if (end > first) {
    cursorCopy = first;
    dx = 971; dy = -841;
    status = AUDIO_CODEC_FUNC(MP3_HuffmanPair)(data, sizeof data,
        &cursorCopy, end - 1, table, &dx, &dy);
    assert(status != STATUS_CONST(SUCCESS));
    assert(cursorCopy == first && dx == 971 && dy == -841);
  }
}
static void checkQuad(unsigned int table, unsigned int symbol) {
  const uint16_t *codes = table == 32 ?
      AUDIO_INS(MP3QuadCodes0) : AUDIO_INS(MP3QuadCodes1);
  const unsigned char *lengths = table == 32 ?
      AUDIO_INS(MP3QuadLengths0) : AUDIO_INS(MP3QuadLengths1);
  unsigned char data[8] = {0};
  size_t first = 2, bit = first, cursor;
  int32_t quad[4] = {9,9,9,9};
  unsigned int i, expected[4];
  for (i = 0; i < 4; ++i) {
    unsigned int present = (symbol >> (3U-i)) & 1U;
    expected[i] = present ? ((i + symbol) & 1U) ? (unsigned int)-1 : 1U : 0U;
  }
  putBits(data, &bit, codes[symbol], lengths[symbol]);
  for (i = 0; i < 4; ++i)
    if ((symbol >> (3U-i)) & 1U)
      putBits(data, &bit, ((i + symbol) & 1U), 1);
  cursor = first;
  assert(AUDIO_CODEC_FUNC(MP3_HuffmanCount1)(data, sizeof data,
      &cursor, bit, table, quad) == STATUS_CONST(SUCCESS));
  assert(cursor == bit);
  for (i = 0; i < 4; ++i)
    assert(quad[i] == (int32_t)expected[i]);
  if (bit > first) {
    cursor = first;
    assert(AUDIO_CODEC_FUNC(MP3_HuffmanCount1)(data, sizeof data,
        &cursor, bit - 1, table, quad) != STATUS_CONST(SUCCESS));
    assert(cursor == first);
  }
}
int main(void) {
  unsigned int table, sym, linbits;
  AUDIO_INS(TMP3Book) book;
  unsigned char data[5] = {0,1,1,1,0};
  size_t cursor = 0;
  int32_t x = 111, y = 222, quad[4] = {0};
  assert(AUDIO_CODEC_FUNC(MP3_HuffmanPair)(data, sizeof data, &cursor,
      sizeof data * 8, 0, &x, &y) == STATUS_CONST(SUCCESS));
  assert(x == 0 && y == 0 && cursor == 0);
  assert(AUDIO_CODEC_FUNC(MP3_HuffmanPair)(data, sizeof data, &cursor,
      sizeof data * 8, 4, &x, &y) == STATUS_CONST(INVALID_ARGUMENT));
  assert(AUDIO_CODEC_FUNC(MP3_HuffmanPair)(data, sizeof data, &cursor,
      sizeof data * 8, 14, &x, &y) == STATUS_CONST(INVALID_ARGUMENT));
  assert(AUDIO_CODEC_FUNC(MP3_HuffmanCount1)(data, sizeof data, &cursor,
      sizeof data * 8, 31, quad) == STATUS_CONST(INVALID_ARGUMENT));
  for (table = 1; table <= 31; ++table) {
    if (table == 4 || table == 14) continue;
    assert(AUDIO_INS(MP3_BookFor)(table, &book, &linbits));
    for (sym = 0; sym < book.symbols; ++sym)
      checkPair(table, sym);
  }
  for (table = 32; table <= 33; ++table)
    for (sym = 0; sym < 16; ++sym)
      checkQuad(table, sym);
  /* Standard fixture independent of stored codebook indexing:
   * Table 2: symbol (1,0) is codeword 011 followed by sign 1 => -1,0. */
  {
    const unsigned char known[] = {0x70}; /* 0111.... */
    cursor = 0;
    assert(AUDIO_CODEC_FUNC(MP3_HuffmanPair)(known, sizeof known,
        &cursor, 4, 2, &x, &y) == STATUS_CONST(SUCCESS));
    assert(cursor == 4 && x == -1 && y == 0);
  }
  {
    AUDIO_CODEC_TYPE(TMP3SideInfo) side = {0};
    AUDIO_CODEC_TYPE(TMP3Scalefactors) scale = {0};
    unsigned char packed[16] = {0};
    int32_t spectrum[576], expected[8] = {-1,0,0,1,-1,0,0,1};
    size_t bit = 0, produced = 0, i;
    side.channels = 1;
    side.granules = 2;
    side.parts[0][0].bigValues = 2;
    side.parts[0][0].tableSelect[0] = 1;
    side.parts[0][0].count1TableSelect = 1;
    side.parts[0][0].part23Length = 15;
    /* Pair (1,0): 01 + sign 1. Pair (0,1): 001 + sign 0. */
    putBits(packed, &bit, 1, 2);
    putBits(packed, &bit, 1, 1);
    putBits(packed, &bit, 1, 3);
    putBits(packed, &bit, 0, 1);
    /* Count1 B nibble 1001 => code 0110, signs for v/y = 1/0. */
    putBits(packed, &bit, 6, 4);
    putBits(packed, &bit, 1, 1);
    putBits(packed, &bit, 0, 1);
    putBits(packed, &bit, 0, 2); /* final stuffing */
    assert(bit == 15);
    assert(AUDIO_CODEC_FUNC(MP3_HuffmanGranule)(
        packed, sizeof packed, &side, &scale, 0, 0, 44100,
        spectrum, &produced) == STATUS_CONST(SUCCESS));
    assert(produced == 8);
    for (i = 0; i < 8; ++i) assert(spectrum[i] == expected[i]);
    for (i = 8; i < 576; ++i) assert(spectrum[i] == 0);
    side.parts[0][0].part23Length = 5;
    spectrum[0] = 12345;
    assert(AUDIO_CODEC_FUNC(MP3_HuffmanGranule)(
        packed, sizeof packed, &side, &scale, 0, 0, 44100,
        spectrum, &produced) == STATUS_CONST(INCOMPLETE_SEQUENCE));
    assert(spectrum[0] == 12345);
    side.parts[0][0].part23Length = 15;
    assert(AUDIO_CODEC_FUNC(MP3_HuffmanGranule)(
        packed, sizeof packed, &side, &scale, 0, 0, 12345,
        spectrum, &produced) == STATUS_CONST(NOT_SUPPORTED));
    side.parts[0][0].windowSwitching = 1;
    side.parts[0][0].blockType = 2;
    assert(AUDIO_CODEC_FUNC(MP3_HuffmanGranule)(
        packed, sizeof packed, &side, &scale, 0, 0, 44100,
        spectrum, &produced) == STATUS_CONST(SUCCESS));
  }
  {
    FILE *file = fopen("Fixtures/Tone.mp3", "rb");
    unsigned char encoded[65536];
    AUDIO_CODEC_TYPE(TMP3Reservoir) reservoir = {0};
    AUDIO_CODEC_TYPE(TMP3HybridState) hybridState[2];
    AUDIO_CODEC_TYPE(TMP3FrameInfo) header;
    AUDIO_CODEC_TYPE(TMP3SideInfo) side;
    AUDIO_CODEC_TYPE(TMP3Scalefactors) first[2];
    const unsigned char *mainData = NULL;
    size_t bytes, offset = 0, frameCount = 0, unpacked = 0;
    assert(file);
    bytes = fread(encoded, 1, sizeof encoded, file);
    assert(!fclose(file) && bytes > 1000);
    memset(hybridState, 0, sizeof hybridState);
    assert(AUDIO_CODEC_FUNC(MP3_FindFrame)(encoded, bytes, 0,
        &offset, &header) == STATUS_CONST(SUCCESS));
    while (bytes - offset >= 4 && frameCount < 50) {
      size_t mainBytes = 0;
      unsigned int gr, ch;
      OPSTATUS status = AUDIO_CODEC_FUNC(MP3_ParseHeader)(
          encoded + offset, bytes - offset, &header);
      if (status != STATUS_CONST(SUCCESS) ||
          header.frameBytes > bytes - offset) break;
      status = AUDIO_CODEC_FUNC(MP3_ReservoirFeed)(&reservoir,
          encoded + offset, header.frameBytes,
          &mainData, &mainBytes, &side);
      assert(status == STATUS_CONST(SUCCESS) ||
          status == STATUS_CONST(INCOMPLETE_SEQUENCE));
      if (status == STATUS_CONST(SUCCESS) && header.mpegVersion == 1) {
        for (gr = 0; gr < side.granules; ++gr) {
          for (ch = 0; ch < side.channels; ++ch) {
            AUDIO_CODEC_TYPE(TMP3Scalefactors) scale;
            int32_t coefficient[576];
            float spectral[576], subband[576];
            size_t quantized = 0;
            status = AUDIO_CODEC_FUNC(MP3_ParseScalefactors)(
                mainData, mainBytes, &side, gr, ch,
                gr ? &first[ch] : NULL, &scale);
            assert(status == STATUS_CONST(SUCCESS));
            if (gr == 0) first[ch] = scale;
            status = AUDIO_CODEC_FUNC(MP3_HuffmanGranule)(
                mainData, mainBytes, &side, &scale, gr, ch,
                header.sampleRate, coefficient, &quantized);
            assert(status == STATUS_CONST(SUCCESS));
            assert(quantized <= 576 && quantized >= side.parts[gr][ch].bigValues * 2);
            assert(AUDIO_CODEC_FUNC(MP3_Requantize)(
                coefficient, &side.parts[gr][ch], &scale, header.sampleRate,
                spectral) == STATUS_CONST(SUCCESS));
            assert(AUDIO_CODEC_FUNC(MP3_ReorderShort)(
                spectral, &side.parts[gr][ch], header.sampleRate,
                spectral) == STATUS_CONST(SUCCESS));
            assert(AUDIO_CODEC_FUNC(MP3_Antialias)(
                spectral, &side.parts[gr][ch], spectral) ==
                STATUS_CONST(SUCCESS));
            assert(AUDIO_CODEC_FUNC(MP3_IMDCT)(
                spectral, &side.parts[gr][ch], &hybridState[ch],
                subband) == STATUS_CONST(SUCCESS));
            assert(AUDIO_CODEC_FUNC(MP3_FrequencyInvert)(
                subband, subband) == STATUS_CONST(SUCCESS));
            ++unpacked;
          }
        }
      }
      offset += header.frameBytes;
      ++frameCount;
    }
    assert(frameCount > 3 && unpacked > 0);
  }
  return 0;
}
