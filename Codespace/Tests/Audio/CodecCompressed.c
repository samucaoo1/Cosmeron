#include "../../Cosmeron/Modules/Audio/CodecMinimal.h"
/*
 * Test-only sound recordings synthesized from a 440 Hz sine, mono, 8000 Hz,
 * 0.18 seconds, using FFmpeg's libmp3lame and FLAC encoders. Generated
 * audio carries no third-party copyrighted material. The FLAC metadata
 * padding was removed using metaflac. Neither encoder is needed in CI.
 */
static const char flac_hex[] =
  "664c614300000022024002400000a600012001f400f0000005a0755de9210a9a6b934869b29edcfee12a8400002c0c0000004c61"
  "766636312e372e3130330100000014000000656e636f6465723d4c61766636312e372e313033fff8240800ca4e0000056b0a320d"
  "c50fb60fcd0e050a94e6310a87d8a5db2c393d0e1b82c67dc68436a945846284308d54b118968b2d2cba92916514a9308c5942c9"
  "4956565a5148a588caac4c2305a2929646134c118524a26138acb2a204249cd0a5326041008852216724290a1ca6984402924220"
  "410080869cc280410302020104f28104094850a4a179321430ce4402081430284c9d0d0a4c4a452c46556261182d1494b2309a60"
  "8c29251309c5659516492a565ad4942309969972a48b45952d5a8985a48988c2195aa516118a10c23552c40081290a14942f2642"
  "8619c880410286050993a1a1498643281043d022010400a4326508201122004100c930880461a14385092734294c981040221488"
  "59809775fff8240801cd4ef187f013f081f2c5f699fb890101065ae632b4853a875bae28693ee5e206fe4e8001a5253088070c22"
  "04402092270c281040c208046685022042240886489cc99028667220104330a1327434290c386502092940881040381929422011"
  "280410264284408a062053819339885332604100a650b39214850d29298440386110220104913861408206104023342811021120"
  "443244e64c81433391008219850993a1a14861c32810494a04408201c0c94a110089402081321422045031029c0c99cc42999302"
  "080532859c90a42869494c2201c3088110082489c30a041030820119a1408810890221922732640a199c880410cc284c9d0d0a43"
  "0e1940824a502204100e064a5088044a01040990a11022818814e064ce4064f8fff8740802011f0b4e0c5408250300fd80f84cf4"
  "01f121f003e59946bcd06e235a68fcce9ae1b83ece002448230462699394911445974eb5160b2244c430818bd4890b0461620c21"
  "ae961304a22c9a9a6a8924285977c610c26285894abd79649092296432fb13046026452b990c23260460a24a2310e5e58a428525"
  "d4d32c911208c1189a64e524451165d3ad4582c891310c2062f52242c1185883086ba584c1288b2072e2";
static const char mp3_hex[] =
  "ffe338c4002b00d260015918015debbd77aef5dec4d9db96efb90d61862ec5d8bb19c390ee396ced61d40d41d75c8f8e1a603713"
  "05137693cf93f7d3e733a5d0536e2bf7002ec5d8ce1dc872c679cae371b97d39e780003830cff000f7fc70c00740000f0f0f0f0c"
  "000000000f0f0f0f0c000000000f0f0f0f0c000000000f0f0f0f0c000000000f0f0f0f0c000000000f0f0f0f0c000000000f0f0f"
  "0f0c000000000f0f0f0f0c000000101e1e1e1e90000001c9c3c3dffc000084100601a050605a05c607a0a8602006756cd6adfffe"
  "607408260a80ea60ffe338c41f32632a48319ea003420a460ca0f56a66ecb7fffcc2d43f0c13025cd658998c7d45c224604801c6"
  "b3a4386180049e602a09269e08da615606e06bb581eaba0752e818b5006ad301a75864e8a2c951f0042606509819232024201890"
  "c06245037440c00153afffe0600186cc0dc40dac1fa85c285a089b8315062afffebf15d10543e118e1410a08748ad45ca4385cc3"
  "35fffffdc991cd1cd2911622c62454ba645e365977fffffd5d9492745598aa648a25d0a5120007132a06bbfcd76e735db9fcfb59"
  "7fdde7ff2fecc0560b90d4618310c328ffe338c42023d3a215719f3800076b4603306be64f84a4260d883b7e607c86f063e34af4"
  "605c8349075b608ec87026f77757b231565646461d662c9b2d78e71f7f3b8e3a72bc7cc3efd50b4b9e65af7b2b98b75eaca5d1ca"
  "3dd2558af4942e527cc94e942e95bc7f9d311968f62afecd5d590bd95d3a3dd5ed2f70ead500820321930e772c62cd57b31447da"
  "db9acae4b2cc9114948980806605046312ca52d302c0b36be64bc20a0098a8d263c2319f420f14331250d4939c77e07b32f3c643"
  "28f2db18e216a5419c18c3d356b476478cbe7f1b76f1334cffe338c45b423bbe70599cc800000e806314404029140208bab8e533"
  "8e5338e56efe36efe36efef35362d321257517892157520f262b2eab8eaae3aabbd55eef3eef3efe7dfccb448055ac5c64555ac8"
  "024895ec8024895fd8eaae3aab8eaae3aabdfcfbf9f7f3eff7fd1493057f23f29ab1647e53563c9d4ae54fa752b9c75571d55c75"
  "571d55c759f779fef3fde7fbefefbfb56b407238aaf43649155e8e4922a8d1c92e551a6b266a934d64cdab8eaae3abb8eaee3abb"
  "8eaee2b3e7d77af62c27da851b2f5ec5c3e7b5ad75f2c2e28916d1c2b04a84d8ffe338c41d2dbb41b401cf3001e23b47a59c8290"
  "9642543d4712247a5693aa1889e34954da72a40a1300c021c2402b22460180491a01d348c122491a474d45891248d23a6a2c712a"
  "348e9a8b1c4aa48eccb1c4aa48eccb1c4b648eccb1c4b648eccfa25b247667d254e8eccfa4b5d1d99f496b91d99f496b91d99f55"
  "af3b33eab5e7667d56ba3a0b90d8a94d0a6c582e4362a53429b160b90d8a9450536682a21b1711414d9a0b4c414d45332e313030"
  "55555555555555555555555555555555555555555555555555555555555555555555555555555555";

static int digit(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return -1;
}
static int decode_hex(const char *hex, unsigned char *dst, size_t capacity) {
  size_t i;
  for (i = 0; i < capacity; ++i) {
    int hi = digit(hex[i * 2]), lo = digit(hex[i * 2 + 1]);
    if (hi < 0 || lo < 0) return 0;
    dst[i] = (unsigned char)((hi << 4) | lo);
  }
  return 1;
}
static int exercise(const unsigned char *data, size_t size) {
  AUDIO_CODEC_TYPE(TDecoder) decoder = {0};
  AUDIO_CODEC_TYPE(TInfo) info = {0};
  float pcm[512] = {0};
  double energy = 0.0;
  size_t iteration;
  size_t frames = 0, total = 0;
  if (AUDIO_CODEC_FUNC(OpenMemory)(&decoder, data, size) !=
      STATUS_CONST(SUCCESS)) return 1;
  if (AUDIO_CODEC_FUNC(GetInfo)(&decoder, &info) !=
      STATUS_CONST(SUCCESS) ||
      info.sampleRate != 8000 || info.channels != 1) {
    AUDIO_CODEC_FUNC(Close)(&decoder);
    return 2;
  }
  for (iteration = 0; iteration < 8; ++iteration) {
    size_t i;
    if (AUDIO_CODEC_FUNC(ReadFrames)(&decoder, pcm, 512, &frames) !=
        STATUS_CONST(SUCCESS)) {
      AUDIO_CODEC_FUNC(Close)(&decoder);
      return 3;
    }
    for (i = 0; i < frames; ++i) {
      float x = pcm[i];
      if (!(x > -2.0f && x < 2.0f)) {
        AUDIO_CODEC_FUNC(Close)(&decoder);
        return 4;
      }
      energy += (x >= 0.f ? x : -x);
    }
    total += frames;
    if (frames == 0) break;
  }
  AUDIO_CODEC_FUNC(Close)(&decoder);
  AUDIO_CODEC_FUNC(Close)(&decoder);
  return (total >= 256 && energy > 0.1) ? 0 : 5;
}
int main(void) {
  unsigned char flac_bytes[(sizeof flac_hex - 1) / 2];
  unsigned char mp3_bytes[(sizeof mp3_hex - 1) / 2];
  if (!decode_hex(flac_hex, flac_bytes, sizeof flac_bytes) ||
      !decode_hex(mp3_hex, mp3_bytes, sizeof mp3_bytes))
    return 1;
  {
    int result = exercise(flac_bytes, sizeof flac_bytes);
    if (result) return 10 + result;
  }
  {
    int result = exercise(mp3_bytes, sizeof mp3_bytes);
    if (result) return 20 + result;
  }
  return 0;
}
