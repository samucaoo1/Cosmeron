#pragma once

#include "../Compiling.inc"

/*
 * Primary compiler family.
 *
 * Compatibility frontends often expose more than one vendor macro
 * (for example clang-cl also defines _MSC_VER). Keep these public
 * family flags mutually exclusive while allowing unknown C11 compilers.
 */
#if defined(__INTEL_COMPILER) || defined(__INTEL_LLVM_COMPILER)
#define COMPILER_INTEL 1
#else
#define COMPILER_INTEL 0
#endif

#if defined(__clang__) && !COMPILER_INTEL
#define COMPILER_CLANG 1
#else
#define COMPILER_CLANG 0
#endif

#if defined(_MSC_VER) && !COMPILER_INTEL && !COMPILER_CLANG
#define COMPILER_MSVC 1
#else
#define COMPILER_MSVC 0
#endif

#if defined(__GNUC__) && !COMPILER_INTEL && !COMPILER_CLANG && !COMPILER_MSVC
#define COMPILER_GCC 1
#else
#define COMPILER_GCC 0
#endif

#define COMPILER_KNOWN                                                        \
  (COMPILER_CLANG || COMPILER_GCC || COMPILER_MSVC || COMPILER_INTEL)
/* EOF */
