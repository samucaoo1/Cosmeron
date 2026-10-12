#pragma once
/* IDs are borrowed only for the duration of the callback. Copy an ID to
 * select it later with OpenNamed. Returning an error stops enumeration. */
AUDIO_DEVICE_ENUMERATE_PROTOTYPE {
  OPSTATUS status;
  if (!callback) return STATUS_CONST(INVALID_ARGUMENT);
  status=callback(userData,"default","Default output");
  if (status!=STATUS_CONST(SUCCESS)) return status;
#if defined(__linux__)
  {
    void *library=dlopen("libasound.so.2",RTLD_NOW|RTLD_LOCAL), *symbol;
    int (*hints)(int,const char *,void ***);
    char *(*get)(const void *,const char *);
    int (*release)(void **);
    void **list=NULL; size_t i;
    if (!library) return STATUS_CONST(NOT_AVAILABLE);
#define AUDIO_DEVICE_ENUM_SYMBOL(F,N) do { symbol=dlsym(library,N); if (!symbol || sizeof symbol!=sizeof F) { dlclose(library); return STATUS_CONST(NOT_AVAILABLE); } memcpy(&F,&symbol,sizeof symbol); } while(0)
    AUDIO_DEVICE_ENUM_SYMBOL(hints,"snd_device_name_hint");
    AUDIO_DEVICE_ENUM_SYMBOL(get,"snd_device_name_get_hint");
    AUDIO_DEVICE_ENUM_SYMBOL(release,"snd_device_name_free_hint");
#undef AUDIO_DEVICE_ENUM_SYMBOL
    if (hints(-1,"pcm",&list)<0) { dlclose(library); return STATUS_CONST(NOT_AVAILABLE); }
    for(i=0;list && list[i];++i) {
      char *name=get(list[i],"NAME"), *description=get(list[i],"DESC"), *io=get(list[i],"IOID");
      if (name && strcmp(name,"default") && (!io || strcmp(io,"Input")))
        status=callback(userData,name,description ? description : name);
      free(name); free(description); free(io);
      if (status!=STATUS_CONST(SUCCESS)) break;
    }
    if (list) release(list);
    dlclose(library);
  }
#elif defined(_WIN32)
  {
    HMODULE library=LoadLibraryA("winmm.dll"); FARPROC symbol;
    UINT (WINAPI *count)(void);
    MMRESULT (WINAPI *caps)(UINT_PTR,LPWAVEOUTCAPSA,UINT);
    UINT i, n;
    if (!library) return STATUS_CONST(NOT_AVAILABLE);
    symbol=GetProcAddress(library,"waveOutGetNumDevs");
    if (!symbol || sizeof symbol!=sizeof count) { FreeLibrary(library); return STATUS_CONST(NOT_AVAILABLE); }
    memcpy(&count,&symbol,sizeof symbol);
    symbol=GetProcAddress(library,"waveOutGetDevCapsA");
    if (!symbol || sizeof symbol!=sizeof caps) { FreeLibrary(library); return STATUS_CONST(NOT_AVAILABLE); }
    memcpy(&caps,&symbol,sizeof symbol);
    n=count();
    for(i=0;i<n;++i) {
      WAVEOUTCAPSA info; char id[32];
      if (caps(i,&info,sizeof info)) continue;
      snprintf(id,sizeof id,"%u",i);
      status=callback(userData,id,info.szPname);
      if (status!=STATUS_CONST(SUCCESS)) break;
    }
    FreeLibrary(library);
  }
#elif defined(__APPLE__)
  {
    void *library=dlopen("/System/Library/Frameworks/CoreAudio.framework/CoreAudio",RTLD_NOW|RTLD_LOCAL), *symbol;
    OSStatus (*size)(AudioObjectID,const AudioObjectPropertyAddress *,UInt32,const void *,UInt32 *);
    OSStatus (*get)(AudioObjectID,const AudioObjectPropertyAddress *,UInt32,const void *,UInt32 *,void *);
    AUDIO_INS(TDeviceCF) cf;
    AudioObjectPropertyAddress property={kAudioHardwarePropertyDevices,kAudioObjectPropertyScopeGlobal,kAudioObjectPropertyElementMain};
    AudioDeviceID *devices; UInt32 bytes=0, i;
    if (!library) return STATUS_CONST(NOT_AVAILABLE);
    symbol=dlsym(library,"AudioObjectGetPropertyDataSize");
    if (!symbol || sizeof symbol!=sizeof size) { dlclose(library); return STATUS_CONST(NOT_AVAILABLE); }
    memcpy(&size,&symbol,sizeof symbol);
    symbol=dlsym(library,"AudioObjectGetPropertyData");
    if (!symbol || sizeof symbol!=sizeof get) { dlclose(library); return STATUS_CONST(NOT_AVAILABLE); }
    memcpy(&get,&symbol,sizeof symbol);
    if (!AUDIO_INS(DeviceCFOpen)(&cf)) { dlclose(library); return STATUS_CONST(NOT_AVAILABLE); }
    if (size(kAudioObjectSystemObject,&property,0,NULL,&bytes) || bytes>1024*1024) {
      dlclose(cf.library); dlclose(library); return STATUS_CONST(NOT_AVAILABLE);
    }
    devices=(AudioDeviceID *)malloc(bytes ? bytes : 1);
    if (!devices) { dlclose(cf.library); dlclose(library); return STATUS_CONST(OUT_OF_MEMORY); }
    if (get(kAudioObjectSystemObject,&property,0,NULL,&bytes,devices)) status=STATUS_CONST(NOT_AVAILABLE);
    else for(i=0;i<bytes/sizeof *devices;++i) {
      CFStringRef uid=NULL, name=NULL; UInt32 n=sizeof uid;
      char id[256], text[256];
      property.mSelector=kAudioDevicePropertyStreams;
      property.mScope=kAudioDevicePropertyScopeOutput;
      if (size(devices[i],&property,0,NULL,&n) || !n) continue;
      property.mSelector=kAudioDevicePropertyDeviceUID;
      property.mScope=kAudioObjectPropertyScopeGlobal; n=sizeof uid;
      if (get(devices[i],&property,0,NULL,&n,&uid) || !uid) continue;
      property.mSelector=kAudioObjectPropertyName; n=sizeof name;
      if (get(devices[i],&property,0,NULL,&n,&name)) name=NULL;
      if (cf.string(uid,id,sizeof id,kCFStringEncodingUTF8)) {
        if (!name || !cf.string(name,text,sizeof text,kCFStringEncodingUTF8)) snprintf(text,sizeof text,"%s",id);
        status=callback(userData,id,text);
      }
      cf.release(uid); if (name) cf.release(name);
      if (status!=STATUS_CONST(SUCCESS)) break;
    }
    free(devices); dlclose(cf.library); dlclose(library);
  }
#elif defined(__FreeBSD__) || defined(__DragonFly__)
  {
    unsigned int i;
    for(i=0;i<32;++i) {
      char id[32]; snprintf(id,sizeof id,"/dev/dsp%u",i);
      if (!access(id,F_OK)) status=callback(userData,id,id);
      if (status!=STATUS_CONST(SUCCESS)) break;
    }
  }
#else
  return STATUS_CONST(NOT_SUPPORTED);
#endif
  return status;
}
