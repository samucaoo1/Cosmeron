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

#if OS_LINUX || OS_MAC || OS_FREEBSD
#define OS_UNIX 1
#else
#define OS_UNIX 0
#endif

#if OS_LINUX || OS_MAC || OS_FREEBSD
#define OS_POSIX 1
#else
#define OS_POSIX 0
#endif

#define OS_KNOWN (OS_WINDOWS || OS_MAC || OS_LINUX || OS_FREEBSD)
/* EOF */
