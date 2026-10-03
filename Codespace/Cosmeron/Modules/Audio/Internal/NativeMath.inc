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
  void *handle = dlopen("libm.so.6", RTLD_NOW | RTLD_LOCAL);
  void *symbol;
  if (handle == NULL)
    return;
#define COSMERON_MACRO_INTERNAL_AUDIO_MATH_LOAD(NAME, SYMBOL)                  \
  symbol = dlsym(handle, SYMBOL);                                              \
  if (symbol == NULL || sizeof(symbol) != sizeof(AUDIO_INS(NAME))) {           \
    dlclose(handle);                                                           \
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
  (void)AUDIO_INS(PrepareMath)();
  return AUDIO_INS(SinPtr)(x);
}
static inline double AUDIO_INS(Exp)(double x) {
  (void)AUDIO_INS(PrepareMath)();
  return AUDIO_INS(ExpPtr)(x);
}
static inline double AUDIO_INS(Log)(double x) {
  (void)AUDIO_INS(PrepareMath)();
  return AUDIO_INS(LogPtr)(x);
}
static inline double AUDIO_INS(Sqrt)(double x) {
  (void)AUDIO_INS(PrepareMath)();
  return AUDIO_INS(SqrtPtr)(x);
}
static inline double AUDIO_INS(Pow)(double x, double y) {
  (void)AUDIO_INS(PrepareMath)();
  return AUDIO_INS(PowPtr)(x, y);
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
