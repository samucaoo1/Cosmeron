#pragma once

#include "../Compiling.inc"
#include "OperationSystem.h"
#include <limits.h>

/*
 * C standard library (libc) implementation detection.
 *
 * Provides normalized, mutually exclusive 0/1 macros identifying the
 * underlying C runtime library (glibc, musl, bionic, msvc, apple, bsd, etc.).
 */

#if defined(__UCLIBC__)
#define LIBC_UCLIBC 1
#else
#define LIBC_UCLIBC 0
#endif

#if defined(__NEWLIB__) || defined(_NEWLIB_VERSION)
#define LIBC_NEWLIB 1
#else
#define LIBC_NEWLIB 0
#endif

#if OS_WINDOWS || defined(_MSC_VER) || defined(_UCRT) || defined(__MSVCRT__)
#define LIBC_MSVC 1
#else
#define LIBC_MSVC 0
#endif

#if OS_MAC && !LIBC_MSVC
#define LIBC_APPLE 1
#else
#define LIBC_APPLE 0
#endif

#if OS_BSD && !LIBC_MSVC && !LIBC_APPLE
#define LIBC_BSD 1
#else
#define LIBC_BSD 0
#endif

#if (defined(__BIONIC__) || OS_ANDROID) && !LIBC_MSVC && !LIBC_APPLE && !LIBC_BSD
#define LIBC_BIONIC 1
#else
#define LIBC_BIONIC 0
#endif

#if OS_WASI
#define LIBC_WASI 1
#else
#define LIBC_WASI 0
#endif

#if OS_EMSCRIPTEN && !LIBC_WASI
#define LIBC_EMSCRIPTEN 1
#else
#define LIBC_EMSCRIPTEN 0
#endif

#if defined(__relibc__) || (OS_REDOX && !LIBC_MSVC && !LIBC_APPLE && !LIBC_BSD)
#define LIBC_RELIBC 1
#else
#define LIBC_RELIBC 0
#endif

#if defined(__GLIBC__) && !LIBC_UCLIBC && !LIBC_NEWLIB && !LIBC_MSVC && \
    !LIBC_APPLE && !LIBC_BSD && !LIBC_BIONIC && !LIBC_WASI && !LIBC_EMSCRIPTEN && \
    !LIBC_RELIBC
#define LIBC_GLIBC 1
#else
#define LIBC_GLIBC 0
#endif

/*
 * musl libc intentionally does not define a vendor identification macro.
 * On Linux targets where neither glibc, uClibc, Bionic nor Newlib is active,
 * the C runtime is musl.
 */
#if ((OS_LINUX && !LIBC_GLIBC && !LIBC_UCLIBC && !LIBC_BIONIC && \
      !LIBC_NEWLIB && !LIBC_MSVC && !LIBC_APPLE && !LIBC_BSD && \
      !LIBC_WASI && !LIBC_EMSCRIPTEN && !LIBC_RELIBC)) ||       \
    defined(__MUSL__) || defined(__musl__)
#define LIBC_MUSL 1
#else
#define LIBC_MUSL 0
#endif

#define LIBC_KNOWN                                                            \
  (LIBC_GLIBC || LIBC_MUSL || LIBC_BIONIC || LIBC_MSVC || LIBC_APPLE ||       \
   LIBC_BSD || LIBC_UCLIBC || LIBC_NEWLIB || LIBC_WASI || LIBC_EMSCRIPTEN || \
   LIBC_RELIBC)

/* Convenience aliases matching the header name */
#define LIBRARY_GLIBC LIBC_GLIBC
#define LIBRARY_MUSL LIBC_MUSL
#define LIBRARY_BIONIC LIBC_BIONIC
#define LIBRARY_MSVC LIBC_MSVC
#define LIBRARY_APPLE LIBC_APPLE
#define LIBRARY_BSD LIBC_BSD
#define LIBRARY_UCLIBC LIBC_UCLIBC
#define LIBRARY_NEWLIB LIBC_NEWLIB
#define LIBRARY_WASI LIBC_WASI
#define LIBRARY_EMSCRIPTEN LIBC_EMSCRIPTEN
#define LIBRARY_RELIBC LIBC_RELIBC
#define LIBRARY_KNOWN LIBC_KNOWN

/* EOF */
