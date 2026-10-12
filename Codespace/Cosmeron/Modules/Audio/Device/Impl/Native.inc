#pragma once
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#if defined(_MSC_VER)
#include <windows.h>
typedef volatile LONG AUDIO_INS(TDeviceAtomic);
#define AUDIO_DEVICE_LOAD(P) InterlockedCompareExchange((P),0,0)
#define AUDIO_DEVICE_STORE(P,V) InterlockedExchange((P),(V))
#define AUDIO_DEVICE_TLS __declspec(thread)
#else
#include <stdatomic.h>
typedef atomic_int AUDIO_INS(TDeviceAtomic);
#define AUDIO_DEVICE_LOAD(P) atomic_load(P)
#define AUDIO_DEVICE_STORE(P,V) atomic_store((P),(V))
#define AUDIO_DEVICE_TLS _Thread_local
#endif
#if defined(_WIN32)
#include <windows.h>
#include <mmsystem.h>
#elif defined(__APPLE__)
#include <AudioToolbox/AudioToolbox.h>
#include <dlfcn.h>
#else
#include <dlfcn.h>
#include <pthread.h>
#include <poll.h>
#if defined(__FreeBSD__) || defined(__DragonFly__)
#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#endif
#endif

#if defined(__linux__)
/* ALSA owns a process-global configuration cache. Keep one runtime-loader
 * reference per TU so unloading it cannot orphan that cache on every Close.
 * Atomic publication also supports concurrent device owners. */
static inline void *AUDIO_INS(NativeALSALibrary)(void) {
  static _Atomic(void *) shared;
  void *library=atomic_load(&shared);
  if (!library) {
    void *opened=dlopen("libasound.so.2",RTLD_NOW|RTLD_LOCAL), *expected=NULL;
    if (!opened) return NULL;
    if (!atomic_compare_exchange_strong(&shared,&expected,opened)) {
      dlclose(opened); library=expected;
    } else library=opened;
  }
  return library;
}
#endif
#include "CoreFoundation.inc"

typedef struct AUDIO_INS(TNativeDevice) {
  AUDIO_DEVICE_TYPE(TConfig) config;
  AUDIO_INS(TDeviceAtomic) started, status;
  bool workerCreated;
  float pcm[512*8];
  int16_t integerPCM[512*8];
  char id[256];
#if defined(_WIN32)
  HMODULE library;
  HWAVEOUT output;
  HANDLE event, worker;
  MMRESULT (WINAPI *open)(LPHWAVEOUT,UINT,LPCWAVEFORMATEX,DWORD_PTR,DWORD_PTR,DWORD);
  MMRESULT (WINAPI *prepare)(HWAVEOUT,LPWAVEHDR,UINT);
  MMRESULT (WINAPI *unprepare)(HWAVEOUT,LPWAVEHDR,UINT);
  MMRESULT (WINAPI *write)(HWAVEOUT,LPWAVEHDR,UINT);
  MMRESULT (WINAPI *reset)(HWAVEOUT);
  MMRESULT (WINAPI *close)(HWAVEOUT);
#elif defined(__APPLE__)
  void *library;
  AudioQueueRef output;
  AudioQueueBufferRef buffers[3];
  OSStatus (*open)(const AudioStreamBasicDescription *,AudioQueueOutputCallback,void *,CFRunLoopRef,CFStringRef,UInt32,AudioQueueRef *);
  OSStatus (*allocate)(AudioQueueRef,UInt32,AudioQueueBufferRef *);
  OSStatus (*enqueue)(AudioQueueRef,AudioQueueBufferRef,UInt32,const AudioStreamPacketDescription *);
  OSStatus (*start)(AudioQueueRef,const AudioTimeStamp *);
  OSStatus (*stop)(AudioQueueRef,Boolean);
  OSStatus (*dispose)(AudioQueueRef,Boolean);
  OSStatus (*setProperty)(AudioQueueRef,AudioQueuePropertyID,const void *,UInt32);
#else
  void *library, *threadLibrary;
  pthread_t worker;
  int (*threadCreate)(pthread_t *,const pthread_attr_t *,void *(*)(void *),void *);
  int (*threadJoin)(pthread_t,void **);
#if defined(__linux__)
  void *output;
  int (*open)(void **,const char *,int,int);
  int (*setParams)(void *,int,int,unsigned int,unsigned int,int,unsigned int);
  int (*formatValue)(const char *);
  long (*write)(void *,const void *,unsigned long);
  int (*recover)(void *,int,int);
  int (*prepare)(void *);
  int (*drop)(void *);
  int (*close)(void *);
#else
  int fd;
#endif
#endif
} AUDIO_INS(TNativeDevice);
static AUDIO_DEVICE_TLS AUDIO_INS(TNativeDevice) *AUDIO_INS(NativeCallbackDevice);

static inline void AUDIO_INS(NativeDeviceRender)(AUDIO_INS(TNativeDevice) *s) {
  size_t i, count=512*s->config.channels;
  memset(s->pcm,0,count*sizeof(float));
  AUDIO_INS(NativeCallbackDevice)=s;
  s->config.render(s->config.userData,s->pcm,512,s->config.channels);
  AUDIO_INS(NativeCallbackDevice)=NULL;
  for (i=0;i<count;++i) {
    float f=AUDIO_INS(Finite)(s->pcm[i]) ? AUDIO_INS(Clamp)(s->pcm[i]) : 0;
    s->pcm[i]=f;
    s->integerPCM[i]=f<=-1 ? INT16_MIN : (int16_t)(f*32767);
  }
}
#if defined(_WIN32)
#define AUDIO_DEVICE_SYMBOL(F,N) do { FARPROC symbol=GetProcAddress(s->library,N); if (!symbol || sizeof symbol!=sizeof s->F) goto fail; memcpy(&s->F,&symbol,sizeof symbol); } while(0)
static DWORD WINAPI AUDIO_INS(NativeDeviceWorker)(LPVOID user) {
  AUDIO_INS(TNativeDevice) *s=(AUDIO_INS(TNativeDevice) *)user;
  WAVEHDR headers[2]={{0}};
  int16_t blocks[2][512*8];
  unsigned int prepared=0, i;
  for(i=0;i<2;++i) {
    headers[i].lpData=(LPSTR)blocks[i];
    headers[i].dwBufferLength=512*s->config.channels*sizeof(int16_t);
    if (s->prepare(s->output,&headers[i],sizeof headers[i])) goto error;
    ++prepared;
    AUDIO_INS(NativeDeviceRender)(s);
    if (!AUDIO_DEVICE_LOAD(&s->started)) goto done;
    memcpy(blocks[i],s->integerPCM,headers[i].dwBufferLength);
    if (s->write(s->output,&headers[i],sizeof headers[i])) goto error;
  }
  while (AUDIO_DEVICE_LOAD(&s->started)) {
    bool progressed=false;
    for(i=0;i<2;++i) if (headers[i].dwFlags&WHDR_DONE) {
      AUDIO_INS(NativeDeviceRender)(s);
      if (!AUDIO_DEVICE_LOAD(&s->started)) goto done;
      memcpy(blocks[i],s->integerPCM,headers[i].dwBufferLength);
      if (s->write(s->output,&headers[i],sizeof headers[i])) goto error;
      progressed=true;
    }
    if (!progressed) WaitForSingleObject(s->event,20);
  }
  goto done;
error:
  AUDIO_DEVICE_STORE(&s->status,STATUS_CONST(NOT_AVAILABLE));
done:
  s->reset(s->output);
  for(i=0;i<prepared;++i)
    if (s->unprepare(s->output,&headers[i],sizeof headers[i]))
      AUDIO_DEVICE_STORE(&s->status,STATUS_CONST(GENERIC_ERROR));
  AUDIO_DEVICE_STORE(&s->started,0);
  return 0;
}
#elif defined(__APPLE__)
#define AUDIO_DEVICE_SYMBOL(F,N) do { void *symbol=dlsym(s->library,N); if (!symbol || sizeof symbol!=sizeof s->F) goto fail; memcpy(&s->F,&symbol,sizeof symbol); } while(0)
static void AUDIO_INS(NativeQueueRender)(void *user, AudioQueueRef queue, AudioQueueBufferRef buffer) {
  AUDIO_INS(TNativeDevice) *s=(AUDIO_INS(TNativeDevice) *)user;
  if (!AUDIO_DEVICE_LOAD(&s->started)) return;
  AUDIO_INS(NativeDeviceRender)(s);
  if (!AUDIO_DEVICE_LOAD(&s->started)) return;
  buffer->mAudioDataByteSize=512*s->config.channels*sizeof(float);
  memcpy(buffer->mAudioData,s->pcm,buffer->mAudioDataByteSize);
  if (s->enqueue(queue,buffer,0,NULL)) {
    AUDIO_DEVICE_STORE(&s->status,STATUS_CONST(GENERIC_ERROR));
    AUDIO_DEVICE_STORE(&s->started,0);
  }
}
#else
#define AUDIO_DEVICE_SYMBOL(F,N) do { void *symbol=dlsym(s->library,N); if (!symbol || sizeof symbol!=sizeof s->F) goto fail; memcpy(&s->F,&symbol,sizeof symbol); } while(0)
static void *AUDIO_INS(NativeDeviceWorker)(void *user) {
  AUDIO_INS(TNativeDevice) *s=(AUDIO_INS(TNativeDevice) *)user;
  while (AUDIO_DEVICE_LOAD(&s->started)) {
    size_t done=0;
    AUDIO_INS(NativeDeviceRender)(s);
    while (done<512 && AUDIO_DEVICE_LOAD(&s->started)) {
#if defined(__linux__)
      long n=s->write(s->output,s->pcm+done*s->config.channels,(unsigned long)(512-done));
      if (n==-EAGAIN || n==0) { (void)poll(NULL,0,5); continue; }
      if (n<0) {
        if ((n!=-EPIPE && n!=-EINTR) || s->recover(s->output,(int)n,1)<0) {
          AUDIO_DEVICE_STORE(&s->status,STATUS_CONST(NOT_AVAILABLE));
          AUDIO_DEVICE_STORE(&s->started,0); break;
        }
        continue;
      }
      if ((unsigned long)n>512-done) {
        AUDIO_DEVICE_STORE(&s->status,STATUS_CONST(INVALID_SEQUENCE));
        AUDIO_DEVICE_STORE(&s->started,0); break;
      }
      done+=(size_t)n;
#elif defined(__FreeBSD__) || defined(__DragonFly__)
      size_t bytes=(512-done)*s->config.channels*sizeof(int16_t);
      ssize_t n=write(s->fd,s->integerPCM+done*s->config.channels,bytes);
      if (n<0 && (errno==EAGAIN || errno==EINTR)) { (void)poll(NULL,0,5); continue; }
      if (n<=0 || (size_t)n%(s->config.channels*sizeof(int16_t))) {
        AUDIO_DEVICE_STORE(&s->status,STATUS_CONST(NOT_AVAILABLE));
        AUDIO_DEVICE_STORE(&s->started,0); break;
      }
      done+=(size_t)n/s->config.channels/sizeof(int16_t);
#else
      AUDIO_DEVICE_STORE(&s->status,STATUS_CONST(NOT_SUPPORTED));
      AUDIO_DEVICE_STORE(&s->started,0);
#endif
    }
  }
#if defined(__linux__)
  s->drop(s->output);
#endif
  AUDIO_DEVICE_STORE(&s->started,0);
  return NULL;
}
#endif

static inline void AUDIO_INS(NativeDeviceRelease)(AUDIO_INS(TNativeDevice) *s) {
#if defined(_WIN32)
  if (s->output) { s->reset(s->output); s->close(s->output); }
  if (s->event) CloseHandle(s->event);
  if (s->library) FreeLibrary(s->library);
#elif defined(__APPLE__)
  if (s->output) s->dispose(s->output,true);
  if (s->library) dlclose(s->library);
#else
#if defined(__linux__)
  if (s->output) s->close(s->output);
#elif defined(__FreeBSD__) || defined(__DragonFly__)
  if (s->fd>=0) close(s->fd);
#endif
#if !defined(__linux__)
  if (s->library) dlclose(s->library);
#endif
  if (s->threadLibrary) dlclose(s->threadLibrary);
#endif
  free(s);
}
#if defined(__APPLE__)
typedef struct AUDIO_INS(TDeviceMatch) { const char *id; bool found; } AUDIO_INS(TDeviceMatch);
static OPSTATUS AUDIO_INS(NativeDeviceMatch)(void *user, const char *id, const char *name) {
  AUDIO_INS(TDeviceMatch) *match=(AUDIO_INS(TDeviceMatch) *)user;
  (void)name;
  if (!strcmp(id,match->id)) match->found=true;
  return STATUS_CONST(SUCCESS);
}
#endif
static inline OPSTATUS AUDIO_INS(NativeDeviceOpen)(
    AUDIO_INS(TNativeDevice) **out, const AUDIO_DEVICE_TYPE(TConfig) *config,
    const char *id) {
  AUDIO_INS(TNativeDevice) *s=(AUDIO_INS(TNativeDevice) *)calloc(1,sizeof *s);
  if (!s) return STATUS_CONST(OUT_OF_MEMORY);
  s->config=*config;
  if (id && strlen(id)>=sizeof s->id) { free(s); return STATUS_CONST(INVALID_ARGUMENT); }
  snprintf(s->id,sizeof s->id,"%s",id ? id : "default");
  AUDIO_DEVICE_STORE(&s->started,0);
  AUDIO_DEVICE_STORE(&s->status,STATUS_CONST(SUCCESS));
#if defined(_WIN32)
  {
    WAVEFORMATEX format={0};
    UINT index=WAVE_MAPPER;
    if (id && strcmp(id,"default")) {
      char *end; unsigned long n=strtoul(id,&end,10);
      if (!*id || *end || n>=UINT_MAX) goto fail;
      index=(UINT)n;
    }
    s->library=LoadLibraryA("winmm.dll"); if (!s->library) goto fail;
    AUDIO_DEVICE_SYMBOL(open,"waveOutOpen"); AUDIO_DEVICE_SYMBOL(prepare,"waveOutPrepareHeader");
    AUDIO_DEVICE_SYMBOL(unprepare,"waveOutUnprepareHeader"); AUDIO_DEVICE_SYMBOL(write,"waveOutWrite");
    AUDIO_DEVICE_SYMBOL(reset,"waveOutReset"); AUDIO_DEVICE_SYMBOL(close,"waveOutClose");
    s->event=CreateEventA(NULL,FALSE,FALSE,NULL); if (!s->event) goto fail;
    format.wFormatTag=WAVE_FORMAT_PCM; format.nChannels=(WORD)config->channels;
    format.nSamplesPerSec=config->sampleRate; format.wBitsPerSample=16;
    format.nBlockAlign=(WORD)(config->channels*2);
    format.nAvgBytesPerSec=config->sampleRate*format.nBlockAlign;
    if (s->open(&s->output,index,&format,(DWORD_PTR)s->event,0,CALLBACK_EVENT)) goto fail;
  }
#elif defined(__APPLE__)
  {
    AudioStreamBasicDescription format={0}; unsigned int i;
    /* AudioQueue may accept an unknown UID and defer failure until Start.
     * Check CoreAudio enumeration before promising that selection opened. */
    if (id && strcmp(id,"default")) {
      AUDIO_INS(TDeviceMatch) match={id,false};
      if (AUDIO_DEVICE_FUNC(Enumerate)(AUDIO_INS(NativeDeviceMatch),&match)!=STATUS_CONST(SUCCESS) || !match.found) goto fail;
    }

    s->library=dlopen("/System/Library/Frameworks/AudioToolbox.framework/AudioToolbox",RTLD_NOW|RTLD_LOCAL);
    if (!s->library) goto fail;
    AUDIO_DEVICE_SYMBOL(open,"AudioQueueNewOutput"); AUDIO_DEVICE_SYMBOL(allocate,"AudioQueueAllocateBuffer");
    AUDIO_DEVICE_SYMBOL(enqueue,"AudioQueueEnqueueBuffer"); AUDIO_DEVICE_SYMBOL(start,"AudioQueueStart");
    AUDIO_DEVICE_SYMBOL(stop,"AudioQueueStop"); AUDIO_DEVICE_SYMBOL(dispose,"AudioQueueDispose");
    AUDIO_DEVICE_SYMBOL(setProperty,"AudioQueueSetProperty");
    format.mSampleRate=config->sampleRate; format.mFormatID=kAudioFormatLinearPCM;
    format.mFormatFlags=kAudioFormatFlagIsFloat|kAudioFormatFlagIsPacked|kAudioFormatFlagsNativeEndian;
    format.mBytesPerPacket=format.mBytesPerFrame=config->channels*sizeof(float);
    format.mFramesPerPacket=1; format.mChannelsPerFrame=config->channels; format.mBitsPerChannel=32;
    if (s->open(&format,AUDIO_INS(NativeQueueRender),s,NULL,NULL,0,&s->output)) goto fail;
    if (id && strcmp(id,"default")) {
      AUDIO_INS(TDeviceCF) cf;
      CFStringRef uid;
      OSStatus selected;
      if (!AUDIO_INS(DeviceCFOpen)(&cf)) goto fail;
      uid=cf.create(NULL,id,kCFStringEncodingUTF8);
      if (!uid) { dlclose(cf.library); goto fail; }
      selected=s->setProperty(s->output,kAudioQueueProperty_CurrentDevice,&uid,sizeof uid);
      cf.release(uid); dlclose(cf.library);
      if (selected) goto fail;
    }
    for(i=0;i<3;++i) if (s->allocate(s->output,512*config->channels*sizeof(float),&s->buffers[i])) goto fail;
  }
#else
#if defined(__linux__)
  s->library=AUDIO_INS(NativeALSALibrary)(); if (!s->library) goto fail;
  AUDIO_DEVICE_SYMBOL(open,"snd_pcm_open"); AUDIO_DEVICE_SYMBOL(setParams,"snd_pcm_set_params");
  AUDIO_DEVICE_SYMBOL(formatValue,"snd_pcm_format_value"); AUDIO_DEVICE_SYMBOL(write,"snd_pcm_writei");
  AUDIO_DEVICE_SYMBOL(recover,"snd_pcm_recover"); AUDIO_DEVICE_SYMBOL(prepare,"snd_pcm_prepare");
  AUDIO_DEVICE_SYMBOL(drop,"snd_pcm_drop"); AUDIO_DEVICE_SYMBOL(close,"snd_pcm_close");
  if (s->open(&s->output,s->id,0,1)<0) goto fail;
  {
    uint16_t endian=1;
    int format=s->formatValue(*(unsigned char *)&endian ? "FLOAT_LE" : "FLOAT_BE");
    if (format<0 || s->setParams(s->output,format,3,config->channels,config->sampleRate,1,40000)<0) goto fail;
  }
#elif defined(__FreeBSD__) || defined(__DragonFly__)
  s->fd=-1;
  if (!id || !strcmp(id,"default")) snprintf(s->id,sizeof s->id,"/dev/dsp");
  s->fd=open(s->id,O_WRONLY|O_NONBLOCK); if (s->fd<0) goto fail;
  {
    int format=AFMT_S16_NE, channels=(int)config->channels, rate=(int)config->sampleRate;
    if (ioctl(s->fd,SNDCTL_DSP_SETFMT,&format)<0 || format!=AFMT_S16_NE ||
        ioctl(s->fd,SNDCTL_DSP_CHANNELS,&channels)<0 || channels!=(int)config->channels ||
        ioctl(s->fd,SNDCTL_DSP_SPEED,&rate)<0 || rate!=(int)config->sampleRate) goto fail;
  }
#else
  goto fail;
#endif
  {
    void *symbol;
#if defined(__linux__)
    s->threadLibrary=dlopen("libpthread.so.0",RTLD_NOW|RTLD_LOCAL);
#else
    s->threadLibrary=dlopen("libpthread.so",RTLD_NOW|RTLD_LOCAL);
#endif
    if (!s->threadLibrary) goto fail;
    symbol=dlsym(s->threadLibrary,"pthread_create");
    if (!symbol || sizeof symbol!=sizeof s->threadCreate) goto fail;
    memcpy(&s->threadCreate,&symbol,sizeof symbol);
    symbol=dlsym(s->threadLibrary,"pthread_join");
    if (!symbol || sizeof symbol!=sizeof s->threadJoin) goto fail;
    memcpy(&s->threadJoin,&symbol,sizeof symbol);
  }
#endif
  *out=s; return STATUS_CONST(SUCCESS);
fail:
  AUDIO_INS(NativeDeviceRelease)(s); return STATUS_CONST(NOT_AVAILABLE);
}
#undef AUDIO_DEVICE_SYMBOL
static inline OPSTATUS AUDIO_INS(NativeDeviceStop)(AUDIO_INS(TNativeDevice) *s) {
  AUDIO_DEVICE_STORE(&s->started,0);
  if (AUDIO_INS(NativeCallbackDevice)==s) return STATUS_CONST(BUSY);
#if defined(_WIN32)
  if (s->workerCreated) {
    if (WaitForSingleObject(s->worker,INFINITE)!=WAIT_OBJECT_0) return STATUS_CONST(GENERIC_ERROR);
    CloseHandle(s->worker); s->worker=NULL;
  }
#elif defined(__APPLE__)
  if (s->stop(s->output,true)) return STATUS_CONST(GENERIC_ERROR);
#else
  if (s->workerCreated && s->threadJoin(s->worker,NULL)) return STATUS_CONST(GENERIC_ERROR);
#endif
  s->workerCreated=false;
  return STATUS_CONST(SUCCESS);
}
static inline OPSTATUS AUDIO_INS(NativeDeviceStart)(AUDIO_INS(TNativeDevice) *s) {
  if (AUDIO_INS(NativeCallbackDevice)==s) return STATUS_CONST(BUSY);
  if (AUDIO_DEVICE_LOAD(&s->started)) return STATUS_CONST(SUCCESS);
  if (s->workerCreated) {
    OPSTATUS status=AUDIO_INS(NativeDeviceStop)(s);
    if (status!=STATUS_CONST(SUCCESS)) return status;
  }
  AUDIO_DEVICE_STORE(&s->status,STATUS_CONST(SUCCESS));
#if defined(__linux__)
  if (s->prepare(s->output)<0) return STATUS_CONST(NOT_AVAILABLE);
#endif
  AUDIO_DEVICE_STORE(&s->started,1);
#if defined(_WIN32)
  s->worker=CreateThread(NULL,0,AUDIO_INS(NativeDeviceWorker),s,0,NULL);
  if (!s->worker) { AUDIO_DEVICE_STORE(&s->started,0); return STATUS_CONST(NOT_AVAILABLE); }
#elif defined(__APPLE__)
  {
    unsigned int i;
    for(i=0;i<3;++i) AUDIO_INS(NativeQueueRender)(s,s->output,s->buffers[i]);
    if (!AUDIO_DEVICE_LOAD(&s->started) || s->start(s->output,NULL)) {
      AUDIO_DEVICE_STORE(&s->started,0); s->stop(s->output,true); return STATUS_CONST(NOT_AVAILABLE);
    }
  }
#else
  if (s->threadCreate(&s->worker,NULL,AUDIO_INS(NativeDeviceWorker),s)) {
    AUDIO_DEVICE_STORE(&s->started,0); return STATUS_CONST(NOT_AVAILABLE);
  }
#endif
  s->workerCreated=true;
  return STATUS_CONST(SUCCESS);
}
