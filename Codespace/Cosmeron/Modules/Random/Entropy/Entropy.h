#pragma once

#include "Entropy.space"
#include "Pool/Pool.h"
#include "../../../Core/Preprocessor/Detect/OperationSystem.h"

#include <errno.h>
#include <stdint.h>
#include <time.h>

#if OS_WINDOWS
#include <windows.h>
#include <bcrypt.h>
#include <limits.h>
#include <string.h>

typedef NTSTATUS (WINAPI *TRandom_BCryptGenRandom)(
    BCRYPT_ALG_HANDLE, PUCHAR, ULONG, ULONG);

typedef struct TRandom_BCrypt_API {
  HMODULE module;
  TRandom_BCryptGenRandom genRandom;
} TRandom_BCrypt_API;

static inline TRandom_BCrypt_API *RANDOM_ENTROPY_FUNC(_BCryptAPI)(void) {
  static TRandom_BCrypt_API api = {0};
  return &api;
}

static inline BOOL CALLBACK RANDOM_ENTROPY_FUNC(_InitializeBCrypt)(
    PINIT_ONCE initOnce, PVOID parameter, PVOID *context) {
  TRandom_BCrypt_API *api = RANDOM_ENTROPY_FUNC(_BCryptAPI)();
  FARPROC symbolAddress;
  (void)initOnce;
  (void)parameter;
  (void)context;

  api->module = LoadLibraryA("bcrypt.dll");
  if (api->module == NULL)
    return TRUE;

  symbolAddress = GetProcAddress(api->module, "BCryptGenRandom");
  if (symbolAddress == NULL ||
      sizeof(api->genRandom) != sizeof(symbolAddress)) {
    FreeLibrary(api->module);
    memset(api, 0, sizeof(*api));
    return TRUE;
  }

  memcpy(&api->genRandom, &symbolAddress, sizeof(api->genRandom));
  return TRUE;
}

static inline TRandom_BCryptGenRandom
RANDOM_ENTROPY_FUNC(_BCryptGenRandom)(void) {
  static INIT_ONCE once = INIT_ONCE_STATIC_INIT;
  TRandom_BCrypt_API *api = RANDOM_ENTROPY_FUNC(_BCryptAPI)();
  if (!InitOnceExecuteOnce(&once, RANDOM_ENTROPY_FUNC(_InitializeBCrypt),
                           NULL, NULL))
    return NULL;
  return api->genRandom;
}
#elif OS_LINUX
#include <sys/random.h>
#elif OS_MAC || OS_FREEBSD
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#endif

inline static bool RANDOM_ENTROPY_FUNC(System)(void *destination, size_t size);
inline static uint64_t RANDOM_ENTROPY_FUNC(Address)(void);
inline static uint64_t RANDOM_ENTROPY_FUNC(Clock)(void);
inline static uint64_t RANDOM_ENTROPY_FUNC(Jitter)(void);
inline static uint64_t RANDOM_ENTROPY_FUNC(Thread)(void);
inline static uint64_t RANDOM_ENTROPY_FUNC(Time)(void);
inline static uint64_t RANDOM_ENTROPY_FUNC(Collect)(void);

#include "Impl/Entropy.impl"
