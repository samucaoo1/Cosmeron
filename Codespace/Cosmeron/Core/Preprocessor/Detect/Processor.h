#pragma once

#include "../Compiling.inc"

/*
 * Primary processor architecture detection.
 *
 * Provides normalized, mutually exclusive 0/1 macros identifying the target architecture.
 */

#if defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64)
#define PROCESSOR_X64 1
#else
#define PROCESSOR_X64 0
#endif

#if (defined(__i386__) || defined(_M_IX86)) && !PROCESSOR_X64
#define PROCESSOR_X86 1
#else
#define PROCESSOR_X86 0
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define PROCESSOR_ARM64 1
#else
#define PROCESSOR_ARM64 0
#endif

#if (defined(__arm__) || defined(_M_ARM)) && !PROCESSOR_ARM64
#define PROCESSOR_ARM 1
#else
#define PROCESSOR_ARM 0
#endif

#if defined(__riscv)
#define PROCESSOR_RISCV 1
#else
#define PROCESSOR_RISCV 0
#endif

#if defined(__wasm__) || defined(__wasm32__) || defined(__wasm64__)
#define PROCESSOR_WASM 1
#else
#define PROCESSOR_WASM 0
#endif

#if defined(__ppc64__) || defined(__PPC64__) || defined(_ARCH_PPC64)
#define PROCESSOR_PPC64 1
#else
#define PROCESSOR_PPC64 0
#endif

#if (defined(__powerpc__) || defined(__ppc__) || defined(_M_PPC) ||            \
     defined(_ARCH_PPC)) &&                                                    \
    !PROCESSOR_PPC64
#define PROCESSOR_PPC 1
#else
#define PROCESSOR_PPC 0
#endif

#if defined(__mips__) || defined(__mips) || defined(__MIPS__)
#define PROCESSOR_MIPS 1
#else
#define PROCESSOR_MIPS 0
#endif

#if defined(__loongarch__) || defined(__loongarch64)
#define PROCESSOR_LOONGARCH64 1
#else
#define PROCESSOR_LOONGARCH64 0
#endif

#if defined(__s390x__) || defined(__s390__)
#define PROCESSOR_S390X 1
#else
#define PROCESSOR_S390X 0
#endif

#define PROCESSOR_KNOWN                                                        \
  (PROCESSOR_X86 || PROCESSOR_X64 || PROCESSOR_ARM || PROCESSOR_ARM64 ||       \
   PROCESSOR_RISCV || PROCESSOR_WASM || PROCESSOR_PPC || PROCESSOR_PPC64 ||    \
   PROCESSOR_MIPS || PROCESSOR_LOONGARCH64 || PROCESSOR_S390X)
/* EOF */
