#pragma once
#include "../../Audio.space"
/* RFC 1321 digest for FLAC STREAMINFO integrity, not authentication. */
typedef struct AUDIO_INS(TFLACMD5) {
  uint32_t h[4];
  uint64_t bytes;
  unsigned char block[64];
  size_t filled;
} AUDIO_INS(TFLACMD5);
static inline void AUDIO_INS(FLAC_MD5Init)(AUDIO_INS(TFLACMD5) *s) {
  memset(s, 0, sizeof *s);
  s->h[0]=UINT32_C(0x67452301); s->h[1]=UINT32_C(0xefcdab89);
  s->h[2]=UINT32_C(0x98badcfe); s->h[3]=UINT32_C(0x10325476);
}
static inline void AUDIO_INS(FLAC_MD5Block)(AUDIO_INS(TFLACMD5) *s) {
  static const uint32_t k[64] = {
    0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,
    0x698098d8,0x8b44f7af,0xffff5bb1,0x895cd7be,0x6b901122,0xfd987193,0xa679438e,0x49b40821,
    0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,
    0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,0xa9e3e905,0xfcefa3f8,0x676f02d9,0x8d2a4c8a,
    0xfffa3942,0x8771f681,0x6d9d6122,0xfde5380c,0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,
    0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,
    0xf4292244,0x432aff97,0xab9423a7,0xfc93a039,0x655b59c3,0x8f0ccc92,0xffeff47d,0x85845dd1,
    0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391};
  static const unsigned char shift[64] = {
    7,12,17,22,7,12,17,22,7,12,17,22,7,12,17,22,
    5,9,14,20,5,9,14,20,5,9,14,20,5,9,14,20,
    4,11,16,23,4,11,16,23,4,11,16,23,4,11,16,23,
    6,10,15,21,6,10,15,21,6,10,15,21,6,10,15,21};
  uint32_t words[16], a=s->h[0], b=s->h[1], c=s->h[2], d=s->h[3];
  unsigned int i;
  for (i=0;i<16;++i) {
    const unsigned char *p=s->block+4*i;
    words[i]=(uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
  }
  for (i=0;i<64;++i) {
    uint32_t f, g, t;
    if (i<16) { f=(b&c)|(~b&d); g=i; }
    else if (i<32) { f=(d&b)|(~d&c); g=(5*i+1)%16; }
    else if (i<48) { f=b^c^d; g=(3*i+5)%16; }
    else { f=c^(b|~d); g=(7*i)%16; }
    t=a+f+k[i]+words[g];
    a=d; d=c; c=b; b+=(t<<shift[i])|(t>>(32-shift[i]));
  }
  s->h[0]+=a; s->h[1]+=b; s->h[2]+=c; s->h[3]+=d;
}
static inline void AUDIO_INS(FLAC_MD5Update)(AUDIO_INS(TFLACMD5) *s,
    const unsigned char *bytes, size_t n) {
  s->bytes += n;
  while (n) {
    size_t count=64-s->filled;
    if (count>n) count=n;
    memcpy(s->block+s->filled,bytes,count);
    s->filled+=count; bytes+=count; n-=count;
    if (s->filled==64) { AUDIO_INS(FLAC_MD5Block)(s); s->filled=0; }
  }
}
static inline void AUDIO_INS(FLAC_MD5Final)(AUDIO_INS(TFLACMD5) *s,
    unsigned char digest[16]) {
  unsigned char pad[64]={0x80}, length[8];
  uint64_t bits=s->bytes*8;
  unsigned int i;
  for (i=0;i<8;++i) length[i]=(unsigned char)(bits>>(8*i));
  AUDIO_INS(FLAC_MD5Update)(s,pad,s->filled<56 ? 56-s->filled : 120-s->filled);
  AUDIO_INS(FLAC_MD5Update)(s,length,8);
  for (i=0;i<16;++i) digest[i]=(unsigned char)(s->h[i/4]>>(8*(i%4)));
}
