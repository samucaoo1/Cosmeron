#include "../../Cosmeron/Modules/Audio/CodecMinimal.h"
#include <assert.h>
#include <stdio.h>
/* An all-zero encoded right channel must remain silent without MS/intensity.
 * Some reference decoders mishandle the nonstandard MPEG-2.5 8-kHz mixed
 * layout; compare left PCM and report right energy without adopting the bug. */
int main(int argc,char **argv) {
  unsigned char bytes[65536]; float native[128*2], reference[128*2];
  AUDIO_CODEC_TYPE(TDecoder) decoder={0}; AUDIO_CODEC_TYPE(TInfo) info;
  cosmeron_audio_vendor_ma_decoder previous;
  cosmeron_audio_vendor_ma_decoder_config config=cosmeron_audio_vendor_ma_decoder_config_init(cosmeron_audio_vendor_ma_format_f32,0,0);
  FILE *file; size_t size,got=0,i; double xx=0,xy=0,yy=0,right=0;
  assert(argc==2); file=fopen(argv[1],"rb"); assert(file);
  size=fread(bytes,1,sizeof bytes,file); fclose(file);
  assert(AUDIO_CODEC_FUNC(OpenMemory)(&decoder,bytes,size)==STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(GetInfo)(&decoder,&info)==STATUS_CONST(SUCCESS));
  assert(info.channels==2 && info.sampleRate==8000);
  assert(cosmeron_audio_vendor_ma_decoder_init_memory(bytes,size,&config,&previous)==cosmeron_audio_vendor_MA_SUCCESS);
  for (;;) {
    cosmeron_audio_vendor_ma_uint64 n=0;
    assert(AUDIO_CODEC_FUNC(ReadFrames)(&decoder,native,128,&got)==STATUS_CONST(SUCCESS));
    if (!got) break;
    (void)cosmeron_audio_vendor_ma_decoder_read_pcm_frames(&previous,reference,128,&n);
    assert(n==got);
    for(i=0;i<got;++i) {
      double a=native[2*i],b=reference[2*i];
      assert(native[2*i+1]==0);
      xx+=a*a;xy+=a*b;yy+=b*b;right+=(double)reference[2*i+1]*reference[2*i+1];
    }
  }
  assert(xx>0 && yy>0 && xy/xx>0.999 && xy/xx<1.001);
  assert(1-xy*xy/(xx*yy)<1e-5);
  printf("[8-kHz mixed baseline] native right energy=0, reference right energy=%.9g; left residual=%.9g\n",right,1-xy*xy/(xx*yy));
  AUDIO_CODEC_FUNC(Close)(&decoder);cosmeron_audio_vendor_ma_decoder_uninit(&previous);
  return 0;
}
