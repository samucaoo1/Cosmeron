#pragma once
#if defined(__APPLE__)
#include <CoreAudio/CoreAudio.h>
typedef struct AUDIO_INS(TDeviceCF) {
  void *library;
  CFStringRef (*create)(CFAllocatorRef,const char *,CFStringEncoding);
  Boolean (*string)(CFStringRef,char *,CFIndex,CFStringEncoding);
  void (*release)(CFTypeRef);
} AUDIO_INS(TDeviceCF);
static inline bool AUDIO_INS(DeviceCFOpen)(AUDIO_INS(TDeviceCF) *cf) {
  void *symbol;
  memset(cf,0,sizeof *cf);
  cf->library=dlopen("/System/Library/Frameworks/CoreFoundation.framework/CoreFoundation",RTLD_NOW|RTLD_LOCAL);
  if (!cf->library) return false;
#define AUDIO_DEVICE_CF_SYMBOL(F,N) do { symbol=dlsym(cf->library,N); if (!symbol || sizeof symbol!=sizeof cf->F) goto fail; memcpy(&cf->F,&symbol,sizeof symbol); } while(0)
  AUDIO_DEVICE_CF_SYMBOL(create,"CFStringCreateWithCString");
  AUDIO_DEVICE_CF_SYMBOL(string,"CFStringGetCString");
  AUDIO_DEVICE_CF_SYMBOL(release,"CFRelease");
#undef AUDIO_DEVICE_CF_SYMBOL
  return true;
fail:
  dlclose(cf->library); cf->library=NULL; return false;
}
#endif
