#pragma once

/* Desktop-only support. Linux glibc >= 2.34 resolves dl/pthread in libc.
 * libm is retained for the process lifetime: vendor worker threads and objects
 * can outlive the translation unit that first prepared the backend. */
#if defined(__linux__)
#include <dlfcn.h>
#include <pthread.h>
static double (*AUDIO_INS(SinPtr))(double);
static double (*AUDIO_INS(ExpPtr))(double);
static double (*AUDIO_INS(LogPtr))(double);
static double (*AUDIO_INS(SqrtPtr))(double);
static double (*AUDIO_INS(PowPtr))(double, double);
static pthread_once_t AUDIO_INS(MathOnce) = PTHREAD_ONCE_INIT;
static bool AUDIO_INS(MathReady);
static void AUDIO_INS(LoadMath)(void) {
  /* glibc exposes a separate libm.so.6. On musl, math is part of libc,
   * and compatibility libm.so can be an empty linker stub. Fall back to
   * lookup in the running process only when no shared libm can be loaded. */
  void *handle = dlopen("libm.so.6", RTLD_NOW | RTLD_LOCAL);
  bool ownHandle = true;
  void *symbol;
  if (handle == NULL)
    handle = dlopen("libm.so", RTLD_NOW | RTLD_LOCAL);
  if (handle == NULL) {
    handle = RTLD_DEFAULT;
    ownHandle = false;
  }
#define COSMERON_MACRO_INTERNAL_AUDIO_MATH_LOAD(NAME, SYMBOL)                  \
  symbol = dlsym(handle, SYMBOL);                                              \
  if (symbol == NULL || sizeof(symbol) != sizeof(AUDIO_INS(NAME))) {           \
    if (ownHandle) dlclose(handle);                                            \
    return;                                                                    \
  }                                                                            \
  memcpy(&AUDIO_INS(NAME), &symbol, sizeof(symbol))
  COSMERON_MACRO_INTERNAL_AUDIO_MATH_LOAD(SinPtr, "sin");
  COSMERON_MACRO_INTERNAL_AUDIO_MATH_LOAD(ExpPtr, "exp");
  COSMERON_MACRO_INTERNAL_AUDIO_MATH_LOAD(LogPtr, "log");
  COSMERON_MACRO_INTERNAL_AUDIO_MATH_LOAD(SqrtPtr, "sqrt");
  COSMERON_MACRO_INTERNAL_AUDIO_MATH_LOAD(PowPtr, "pow");
#undef COSMERON_MACRO_INTERNAL_AUDIO_MATH_LOAD
  AUDIO_INS(MathReady) = true;
}
static inline bool AUDIO_INS(PrepareMath)(void) {
  return pthread_once(&AUDIO_INS(MathOnce), AUDIO_INS(LoadMath)) == 0 &&
         AUDIO_INS(MathReady);
}
static inline double AUDIO_INS(Sin)(double x) {
  /* Decoder constructors return NOT_AVAILABLE on failed math initialization;
   * guard direct internal adapter calls too, avoiding NULL dereferences. */
  return AUDIO_INS(PrepareMath)() ? AUDIO_INS(SinPtr)(x) : 0.0;
}
static inline double AUDIO_INS(Exp)(double x) {
  /* Decoder constructors return NOT_AVAILABLE on failed math initialization;
   * guard direct internal adapter calls too, avoiding NULL dereferences. */
  return AUDIO_INS(PrepareMath)() ? AUDIO_INS(ExpPtr)(x) : 0.0;
}
static inline double AUDIO_INS(Log)(double x) {
  /* Decoder constructors return NOT_AVAILABLE on failed math initialization;
   * guard direct internal adapter calls too, avoiding NULL dereferences. */
  return AUDIO_INS(PrepareMath)() ? AUDIO_INS(LogPtr)(x) : 0.0;
}
static inline double AUDIO_INS(Sqrt)(double x) {
  /* Decoder constructors return NOT_AVAILABLE on failed math initialization;
   * guard direct internal adapter calls too, avoiding NULL dereferences. */
  return AUDIO_INS(PrepareMath)() ? AUDIO_INS(SqrtPtr)(x) : 0.0;
}
static inline double AUDIO_INS(Pow)(double x, double y) {
  /* Decoder constructors return NOT_AVAILABLE on failed math initialization;
   * guard direct internal adapter calls too, avoiding NULL dereferences. */
  return AUDIO_INS(PrepareMath)() ? AUDIO_INS(PowPtr)(x, y) : 0.0;
}
#elif defined(AUDIO_MINIAUDIO_CODEC_MINIMAL) && \
    (defined(__FreeBSD__) || defined(__OpenBSD__) || \
     defined(__NetBSD__) || defined(__DragonFly__))
/*
 * BSD decoder-only profile:
 *  - the native threadless Miniaudio build still needs math functions;
 *  - dlopen/dlsym come from the target's runtime linker, with no -ldl flag;
 *  - libm is a RUNTIME requirement, not an embedded implementation;
 *  - a C11 atomic state machine makes initialization safe with concurrent
 *    callers without creating a thread or requiring -pthread.
 *
 * The handle stays open for the process lifetime because decoded objects may
 * be shared between translation units. No arbitrary library paths are used.
 */
#include <dlfcn.h>
#include <stdatomic.h>

static double (*AUDIO_INS(SinPtr))(double);
static double (*AUDIO_INS(ExpPtr))(double);
static double (*AUDIO_INS(LogPtr))(double);
static double (*AUDIO_INS(SqrtPtr))(double);
static double (*AUDIO_INS(PowPtr))(double, double);
static void *AUDIO_INS(MathHandle);
/* 0 = not started, 1 = initializing, 2 = ready, 3 = unavailable */
static atomic_int AUDIO_INS(MathState) = ATOMIC_VAR_INIT(0);

static inline bool AUDIO_INS(LoadMathSymbol)(
    void *library, const char *name, void *target, size_t bytes) {
  void *symbol = dlsym(library, name);
  if (symbol == NULL || bytes != sizeof symbol)
    return false;
  memcpy(target, &symbol, bytes);
  return true;
}
static inline bool AUDIO_INS(LoadMath)(void) {
  static const char *const candidates[] = {
    "libm.so", "libm.so.6", "libm.so.5", "libm.so.4", "libm.so.3",
    "libm.so.2", "libm.so.1", "libm.so.0"
  };
  size_t i;
  void *handle = NULL;
  for (i = 0; i < sizeof candidates / sizeof candidates[0]; ++i) {
    handle = dlopen(candidates[i], RTLD_NOW | RTLD_LOCAL);
    if (handle != NULL) break;
  }
  if (handle == NULL)
    return false;
  if (!AUDIO_INS(LoadMathSymbol)(handle, "sin", &AUDIO_INS(SinPtr),
                                sizeof AUDIO_INS(SinPtr)) ||
      !AUDIO_INS(LoadMathSymbol)(handle, "exp", &AUDIO_INS(ExpPtr),
                                sizeof AUDIO_INS(ExpPtr)) ||
      !AUDIO_INS(LoadMathSymbol)(handle, "log", &AUDIO_INS(LogPtr),
                                sizeof AUDIO_INS(LogPtr)) ||
      !AUDIO_INS(LoadMathSymbol)(handle, "sqrt", &AUDIO_INS(SqrtPtr),
                                sizeof AUDIO_INS(SqrtPtr)) ||
      !AUDIO_INS(LoadMathSymbol)(handle, "pow", &AUDIO_INS(PowPtr),
                                sizeof AUDIO_INS(PowPtr))) {
    dlclose(handle);
    AUDIO_INS(SinPtr) = NULL;
    AUDIO_INS(ExpPtr) = NULL;
    AUDIO_INS(LogPtr) = NULL;
    AUDIO_INS(SqrtPtr) = NULL;
    AUDIO_INS(PowPtr) = NULL;
    return false;
  }
  AUDIO_INS(MathHandle) = handle;
  return true;
}
static inline bool AUDIO_INS(PrepareMath)(void) {
  int state = atomic_load_explicit(&AUDIO_INS(MathState),
                                   memory_order_acquire);
  if (state == 2) return true;
  if (state == 3) return false;
  {
    int expected = 0;
    if (atomic_compare_exchange_strong_explicit(
            &AUDIO_INS(MathState), &expected, 1,
            memory_order_acq_rel, memory_order_acquire)) {
      bool loaded = AUDIO_INS(LoadMath)();
      atomic_store_explicit(&AUDIO_INS(MathState), loaded ? 2 : 3,
                            memory_order_release);
      return loaded;
    }
  }
  /* An earlier caller owns the initialization; no pthread library needed. */
  do {
    state = atomic_load_explicit(&AUDIO_INS(MathState),
                                 memory_order_acquire);
  } while (state == 1);
  return state == 2;
}
/* Guard calls as well as decoder constructors: a missing runtime library must
 * yield NOT_AVAILABLE from the public API, never a NULL function pointer call. */
static inline double AUDIO_INS(Sin)(double x) {
  return AUDIO_INS(PrepareMath)() ? AUDIO_INS(SinPtr)(x) : 0.0;
}
static inline double AUDIO_INS(Exp)(double x) {
  return AUDIO_INS(PrepareMath)() ? AUDIO_INS(ExpPtr)(x) : 0.0;
}
static inline double AUDIO_INS(Log)(double x) {
  return AUDIO_INS(PrepareMath)() ? AUDIO_INS(LogPtr)(x) : 0.0;
}
static inline double AUDIO_INS(Sqrt)(double x) {
  return AUDIO_INS(PrepareMath)() ? AUDIO_INS(SqrtPtr)(x) : 0.0;
}
static inline double AUDIO_INS(Pow)(double x, double y) {
  return AUDIO_INS(PrepareMath)() ? AUDIO_INS(PowPtr)(x, y) : 0.0;
}
#else
#include <math.h>
static inline bool AUDIO_INS(PrepareMath)(void) { return true; }
static inline double AUDIO_INS(Sin)(double x) { return sin(x); }
static inline double AUDIO_INS(Exp)(double x) { return exp(x); }
static inline double AUDIO_INS(Log)(double x) { return log(x); }
static inline double AUDIO_INS(Sqrt)(double x) { return sqrt(x); }
static inline double AUDIO_INS(Pow)(double x, double y) { return pow(x, y); }
#endif
