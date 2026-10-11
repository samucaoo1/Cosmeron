#pragma once

#include "../Compiling.inc"

#if defined(_WIN32) || defined(_WIN64)
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

#define OS_BSD (OS_FREEBSD || OS_OPENBSD || OS_NETBSD || OS_DRAGONFLY)
#define OS_UNIX (OS_LINUX || OS_MAC || OS_BSD)
#define OS_POSIX (OS_LINUX || OS_MAC || OS_BSD)
#define OS_KNOWN (OS_WINDOWS || OS_MAC || OS_LINUX || OS_BSD)
/* EOF */
