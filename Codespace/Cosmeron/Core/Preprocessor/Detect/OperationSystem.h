#pragma once

#include "../Compiling.inc"

#if defined(__CYGWIN__)
#define OS_CYGWIN 1
#else
#define OS_CYGWIN 0
#endif

#if (defined(_WIN32) || defined(_WIN64)) && !OS_CYGWIN
#define OS_WINDOWS 1
#else
#define OS_WINDOWS 0
#endif

#if OS_WINDOWS && !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if defined(__APPLE__) && defined(__MACH__)
#define OS_MAC 1
#else
#define OS_MAC 0
#endif

#if OS_MAC && (defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) || \
               defined(__ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__) ||     \
               defined(__ENVIRONMENT_WATCH_OS_VERSION_MIN_REQUIRED__))
#define OS_IOS 1
#else
#define OS_IOS 0
#endif

/* OHOS is a target ABI/userland; its standard profile can also
 * define __linux__. Keep OS_OHOS as an overlay, not a second kernel. */
#if defined(__OHOS__)
#define OS_OHOS 1
#else
#define OS_OHOS 0
#endif

/* Android is a target ABI/userland on top of the Linux kernel.
 * Keep OS_ANDROID as an overlay, aligned with OS_OHOS. */
#if defined(__ANDROID__)
#define OS_ANDROID 1
#else
#define OS_ANDROID 0
#endif

#if defined(__linux__) || defined(__linux)
#define OS_LINUX 1
#else
#define OS_LINUX 0
#endif

#if defined(__FreeBSD__) || defined(__FreeBSD)
#define OS_FREEBSD 1
#else
#define OS_FREEBSD 0
#endif

#if defined(__OpenBSD__)
#define OS_OPENBSD 1
#else
#define OS_OPENBSD 0
#endif

#if defined(__NetBSD__)
#define OS_NETBSD 1
#else
#define OS_NETBSD 0
#endif

#if defined(__DragonFly__)
#define OS_DRAGONFLY 1
#else
#define OS_DRAGONFLY 0
#endif

#if defined(sun) || defined(__sun) || defined(__sun__)
#define OS_SOLARIS 1
#else
#define OS_SOLARIS 0
#endif

#if defined(_AIX) || defined(__TOS_AIX__)
#define OS_AIX 1
#else
#define OS_AIX 0
#endif

#if defined(__HAIKU__) || defined(__BEOS__)
#define OS_HAIKU 1
#else
#define OS_HAIKU 0
#endif

#if defined(__QNX__) || defined(__QNXNTO__)
#define OS_QNX 1
#else
#define OS_QNX 0
#endif

#if defined(__GNU__) || defined(__gnu_hurd__)
#define OS_HURD 1
#else
#define OS_HURD 0
#endif

#if defined(__EMSCRIPTEN__)
#define OS_EMSCRIPTEN 1
#else
#define OS_EMSCRIPTEN 0
#endif

#if defined(__wasi__)
#define OS_WASI 1
#else
#define OS_WASI 0
#endif

#if defined(__wasm__) || defined(__wasm32__) || defined(__wasm64__) || \
    OS_EMSCRIPTEN || OS_WASI
#define OS_WASM 1
#else
#define OS_WASM 0
#endif

#define OS_BSD (OS_FREEBSD || OS_OPENBSD || OS_NETBSD || OS_DRAGONFLY)
#define OS_UNIX                                                               \
  (OS_LINUX || OS_MAC || OS_BSD || OS_SOLARIS || OS_AIX || OS_HAIKU ||      \
   OS_QNX || OS_HURD || OS_CYGWIN)
#define OS_POSIX (OS_UNIX || OS_ANDROID || OS_OHOS)
#define OS_KNOWN                                                              \
  (OS_WINDOWS || OS_MAC || OS_LINUX || OS_BSD || OS_OHOS || OS_ANDROID ||     \
   OS_SOLARIS || OS_AIX || OS_HAIKU || OS_QNX || OS_HURD || OS_CYGWIN ||      \
   OS_WASM)
/* EOF */
