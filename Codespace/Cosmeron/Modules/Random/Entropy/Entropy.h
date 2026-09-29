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

typedef NTSTATUS (WINAPI *RANDOM_ENTROPY_INS(BCryptGenRandom_InternalType_))(
    BCRYPT_ALG_HANDLE, PUCHAR, ULONG, ULONG);

typedef struct RANDOM_ENTROPY_INS(BCryptAPI_InternalType_) {
  HMODULE module;
  RANDOM_ENTROPY_INS(BCryptGenRandom_InternalType_) genRandom;
} RANDOM_ENTROPY_INS(BCryptAPI_InternalType_);

static inline RANDOM_ENTROPY_INS(BCryptAPI_InternalType_) *
RANDOM_ENTROPY_INS(BCryptAPI)(void) {
  static RANDOM_ENTROPY_INS(BCryptAPI_InternalType_) api = {0};
  return &api;
}

static inline BOOL CALLBACK RANDOM_ENTROPY_INS(InitializeBCrypt)(
    PINIT_ONCE initOnce, PVOID parameter, PVOID *context) {
  RANDOM_ENTROPY_INS(BCryptAPI_InternalType_) *api =
      RANDOM_ENTROPY_INS(BCryptAPI)();
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

static inline RANDOM_ENTROPY_INS(BCryptGenRandom_InternalType_)
RANDOM_ENTROPY_INS(BCryptGenRandom)(void) {
  static INIT_ONCE once = INIT_ONCE_STATIC_INIT;
  RANDOM_ENTROPY_INS(BCryptAPI_InternalType_) *api =
      RANDOM_ENTROPY_INS(BCryptAPI)();
  if (!InitOnceExecuteOnce(&once, RANDOM_ENTROPY_INS(InitializeBCrypt),
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

#define RANDOM_ENTROPY_SYSTEM_PROTOTYPE                                      \
  static inline OPSTATUS RANDOM_ENTROPY_FUNC(System)(                         \
      void *destination, size_t size)
#define RANDOM_ENTROPY_ADDRESS_PROTOTYPE                                     \
  static inline uint64_t RANDOM_ENTROPY_FUNC(Address)(void)
#define RANDOM_ENTROPY_CLOCK_PROTOTYPE                                       \
  static inline uint64_t RANDOM_ENTROPY_FUNC(Clock)(void)
#define RANDOM_ENTROPY_JITTER_PROTOTYPE                                      \
  static inline uint64_t RANDOM_ENTROPY_FUNC(Jitter)(void)
#define RANDOM_ENTROPY_THREAD_PROTOTYPE                                      \
  static inline uint64_t RANDOM_ENTROPY_FUNC(Thread)(void)
#define RANDOM_ENTROPY_TIME_PROTOTYPE                                        \
  static inline uint64_t RANDOM_ENTROPY_FUNC(Time)(void)
#define RANDOM_ENTROPY_COLLECT_PROTOTYPE                                     \
  static inline uint64_t RANDOM_ENTROPY_FUNC(Collect)(void)

RANDOM_ENTROPY_SYSTEM_PROTOTYPE;
RANDOM_ENTROPY_ADDRESS_PROTOTYPE;
RANDOM_ENTROPY_CLOCK_PROTOTYPE;
RANDOM_ENTROPY_JITTER_PROTOTYPE;
RANDOM_ENTROPY_THREAD_PROTOTYPE;
RANDOM_ENTROPY_TIME_PROTOTYPE;
RANDOM_ENTROPY_COLLECT_PROTOTYPE;

#include "Impl/Entropy.impl"
