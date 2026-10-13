#pragma once

#include "../Compiling.inc"

#if defined(__i386__) || defined(_M_IX86)
#define PROCESSOR_X86 1
#else
#define PROCESSOR_X86 0
#endif

#if defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64)
#define PROCESSOR_X64 1
#else
#define PROCESSOR_X64 0
#endif

#if defined(__arm__) || defined(_M_ARM)
#define PROCESSOR_ARM 1
#else
#define PROCESSOR_ARM 0
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define PROCESSOR_ARM64 1
#else
#define PROCESSOR_ARM64 0
#endif

#if defined(__riscv)
#define PROCESSOR_RISCV 1
#else
#define PROCESSOR_RISCV 0
#endif

#define PROCESSOR_KNOWN                                                       \
  (PROCESSOR_X86 || PROCESSOR_X64 || PROCESSOR_ARM || PROCESSOR_ARM64 ||      \
   PROCESSOR_RISCV)
/* EOF */
