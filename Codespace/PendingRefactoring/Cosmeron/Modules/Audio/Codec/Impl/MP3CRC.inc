#pragma once
/* ISO MPEG audio CRC-16: protected header low 16 bits and Layer III
 * side information, MSB first, initial 0xffff, polynomial x^16+x^15+x^2+1.
 * Main-data Huffman payload is not covered by this protection word. */
#define AUDIO_CODEC_MP3_CHECK_CRC_PROTOTYPE \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_CheckCRC)(const void *frame, size_t size)
AUDIO_CODEC_MP3_CHECK_CRC_PROTOTYPE {
  const unsigned char *p=(const unsigned char *)frame;
  AUDIO_CODEC_TYPE(TMP3FrameInfo) header;
  uint16_t crc=UINT16_C(0xffff), expected;
  size_t i;
  OPSTATUS status=AUDIO_CODEC_FUNC(MP3_ParseHeader)(frame,size,&header);
  if (status!=STATUS_CONST(SUCCESS)) return status;
  if (!header.crcProtected) return STATUS_CONST(SUCCESS);
  if (size<6U+header.sideInfoBytes) return STATUS_CONST(INCOMPLETE_SEQUENCE);
  expected=(uint16_t)(((uint16_t)p[4]<<8)|p[5]);
  for (i=0;i<2U+header.sideInfoBytes;++i) {
    unsigned char byte=p[i<2 ? i+2 : i+4];
    unsigned int bit;
    for(bit=0;bit<8;++bit) {
      unsigned int feedback=((crc>>15)^(byte>>(7-bit)))&1U;
      crc=(uint16_t)(crc<<1);
      if (feedback) crc^=UINT16_C(0x8005);
    }
  }
  return crc==expected ? STATUS_CONST(SUCCESS) : STATUS_CONST(INVALID_SEQUENCE);
}
