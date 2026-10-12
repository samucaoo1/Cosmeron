#include "../../Cosmeron/Modules/Audio/CodecMinimal.h"
#include <stdio.h>
#include <time.h>
#if defined(__linux__)
#include <sys/resource.h>
#endif
int main(int argc,char **argv) {
  unsigned char *bytes; size_t size; FILE *file; long length;
  unsigned int pass; clock_t start; uint64_t total=0; double sum=0;
  float pcm[512*8];
  if (argc!=3) return 2;
  file=fopen(argv[2],"rb"); if (!file || fseek(file,0,SEEK_END)) return 2;
  length=ftell(file); if (length<=0 || fseek(file,0,SEEK_SET)) return 2;
  size=(size_t)length; bytes=(unsigned char *)malloc(size); if (!bytes) return 2;
  if (fread(bytes,1,size,file)!=size) return 2;
  fclose(file); start=clock();
  for(pass=0;pass<5;++pass) {
    if (!strcmp(argv[1],"native")) {
      AUDIO_CODEC_TYPE(TDecoder) decoder={0}; AUDIO_CODEC_TYPE(TInfo) info;
      if (AUDIO_CODEC_FUNC(OpenMemory)(&decoder,bytes,size)!=STATUS_CONST(SUCCESS)) return 1;
      if (AUDIO_CODEC_FUNC(GetInfo)(&decoder,&info)!=STATUS_CONST(SUCCESS)) return 1;
      for (;;) {
        size_t got=0;
        if (AUDIO_CODEC_FUNC(ReadFrames)(&decoder,pcm,512,&got)!=STATUS_CONST(SUCCESS)) return 1;
        if (!got) break;
        total+=got; sum+=pcm[0];
      }
      AUDIO_CODEC_FUNC(Close)(&decoder);
    } else {
      cosmeron_audio_vendor_ma_decoder decoder;
      cosmeron_audio_vendor_ma_decoder_config config=cosmeron_audio_vendor_ma_decoder_config_init(cosmeron_audio_vendor_ma_format_f32,0,0);
      if (cosmeron_audio_vendor_ma_decoder_init_memory(bytes,size,&config,&decoder)!=cosmeron_audio_vendor_MA_SUCCESS) return 1;
      for (;;) {
        cosmeron_audio_vendor_ma_uint64 got=0;
        cosmeron_audio_vendor_ma_result result=cosmeron_audio_vendor_ma_decoder_read_pcm_frames(&decoder,pcm,512,&got);
        if (result!=cosmeron_audio_vendor_MA_SUCCESS && result!=cosmeron_audio_vendor_MA_AT_END) return 1;
        if (!got) break;
        total+=got; sum+=pcm[0];
      }
      cosmeron_audio_vendor_ma_decoder_uninit(&decoder);
    }
  }
  printf("backend=%s cpu_seconds=%.6f frames=%llu checksum=%.6f",argv[1],
      (double)(clock()-start)/CLOCKS_PER_SEC,(unsigned long long)total,sum);
#if defined(__linux__)
  { struct rusage usage; if (!getrusage(RUSAGE_SELF,&usage)) printf(" peak_rss_kib=%ld",usage.ru_maxrss); }
#endif
  printf("\n"); free(bytes); return 0;
}
