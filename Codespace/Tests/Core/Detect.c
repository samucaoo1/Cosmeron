#include "../../Cosmeron/Core/Preprocessor/Detect/Compiler.h"
#include "../../Cosmeron/Core/Preprocessor/Detect/OperationSystem.h"
#include "../../Cosmeron/Core/Preprocessor/Detect/Processor.h"
#include "../../Cosmeron/Core/Preprocessor/Detect/Library.h"

#if (COMPILER_CLANG + COMPILER_GCC + COMPILER_MSVC + COMPILER_INTEL) > 1
#error "Primary compiler-family detection must be mutually exclusive."
#endif

#if (OS_WINDOWS + OS_MAC + OS_LINUX + OS_FREEBSD + OS_OPENBSD + OS_NETBSD + \
     OS_DRAGONFLY + OS_SOLARIS + OS_AIX + OS_HAIKU + OS_QNX + OS_HURD +      \
     OS_CYGWIN + OS_WASM) > 1
#error "Operating-system detection must identify at most one supported target."
#endif

#if (PROCESSOR_X86 + PROCESSOR_X64 + PROCESSOR_ARM + PROCESSOR_ARM64 +        \
     PROCESSOR_RISCV) > 1
#error "Processor detection must identify at most one architecture."
#endif

#if (LIBC_GLIBC + LIBC_MUSL + LIBC_BIONIC + LIBC_MSVC + LIBC_APPLE +          \
     LIBC_BSD + LIBC_UCLIBC + LIBC_NEWLIB + LIBC_WASI + LIBC_EMSCRIPTEN) > 1
#error "C library detection must identify at most one target implementation."
#endif

#if COMPILER_KNOWN !=                                                        \
    (COMPILER_CLANG || COMPILER_GCC || COMPILER_MSVC || COMPILER_INTEL)
#error "COMPILER_KNOWN must describe, not constrain, compiler detection."
#endif

#if OS_KNOWN !=                                                              \
    (OS_WINDOWS || OS_MAC || OS_LINUX || OS_BSD || OS_OHOS || OS_ANDROID ||   \
     OS_SOLARIS || OS_AIX || OS_HAIKU || OS_QNX || OS_HURD || OS_CYGWIN ||    \
     OS_WASM)
#error "OS_KNOWN must describe, not constrain, operating-system detection."
#endif

#if PROCESSOR_KNOWN !=                                                       \
    (PROCESSOR_X86 || PROCESSOR_X64 || PROCESSOR_ARM || PROCESSOR_ARM64 ||   \
     PROCESSOR_RISCV)
#error "PROCESSOR_KNOWN must describe, not constrain, processor detection."
#endif

#if LIBC_KNOWN !=                                                            \
    (LIBC_GLIBC || LIBC_MUSL || LIBC_BIONIC || LIBC_MSVC || LIBC_APPLE ||    \
     LIBC_BSD || LIBC_UCLIBC || LIBC_NEWLIB || LIBC_WASI || LIBC_EMSCRIPTEN)
#error "LIBC_KNOWN must describe, not constrain, library detection."
#endif

#if OS_UNIX !=                                                               \
    (OS_LINUX || OS_MAC || OS_BSD || OS_SOLARIS || OS_AIX || OS_HAIKU ||      \
     OS_QNX || OS_HURD || OS_CYGWIN)
#error "OS_UNIX classification is inconsistent."
#endif

#if OS_POSIX != (OS_UNIX || OS_ANDROID || OS_OHOS)
#error "OS_POSIX classification is inconsistent."
#endif

#if defined(_MSC_VER)
#pragma warning(disable : 4127)
#endif

#define DETECT_BOOLEAN_VALUE(value) ((value) == 0 || (value) == 1)

int main(void) {
  if (!DETECT_BOOLEAN_VALUE(COMPILER_CLANG) ||
      !DETECT_BOOLEAN_VALUE(COMPILER_GCC) ||
      !DETECT_BOOLEAN_VALUE(COMPILER_MSVC) ||
      !DETECT_BOOLEAN_VALUE(COMPILER_INTEL) ||
      !DETECT_BOOLEAN_VALUE(COMPILER_KNOWN))
    return 1;

  if (!DETECT_BOOLEAN_VALUE(OS_WINDOWS) || !DETECT_BOOLEAN_VALUE(OS_MAC) ||
      !DETECT_BOOLEAN_VALUE(OS_IOS) ||
      !DETECT_BOOLEAN_VALUE(OS_LINUX) || !DETECT_BOOLEAN_VALUE(OS_FREEBSD) ||
      !DETECT_BOOLEAN_VALUE(OS_OPENBSD) || !DETECT_BOOLEAN_VALUE(OS_NETBSD) ||
      !DETECT_BOOLEAN_VALUE(OS_DRAGONFLY) || !DETECT_BOOLEAN_VALUE(OS_BSD) ||
      !DETECT_BOOLEAN_VALUE(OS_OHOS) || !DETECT_BOOLEAN_VALUE(OS_ANDROID) ||
      !DETECT_BOOLEAN_VALUE(OS_SOLARIS) || !DETECT_BOOLEAN_VALUE(OS_AIX) ||
      !DETECT_BOOLEAN_VALUE(OS_HAIKU) || !DETECT_BOOLEAN_VALUE(OS_QNX) ||
      !DETECT_BOOLEAN_VALUE(OS_HURD) || !DETECT_BOOLEAN_VALUE(OS_CYGWIN) ||
      !DETECT_BOOLEAN_VALUE(OS_WASM) || !DETECT_BOOLEAN_VALUE(OS_EMSCRIPTEN) ||
      !DETECT_BOOLEAN_VALUE(OS_WASI) ||
      !DETECT_BOOLEAN_VALUE(OS_UNIX) || !DETECT_BOOLEAN_VALUE(OS_POSIX) ||
      !DETECT_BOOLEAN_VALUE(OS_KNOWN))
    return 2;

  if (!DETECT_BOOLEAN_VALUE(PROCESSOR_X86) ||
      !DETECT_BOOLEAN_VALUE(PROCESSOR_X64) ||
      !DETECT_BOOLEAN_VALUE(PROCESSOR_ARM) ||
      !DETECT_BOOLEAN_VALUE(PROCESSOR_ARM64) ||
      !DETECT_BOOLEAN_VALUE(PROCESSOR_RISCV) ||
      !DETECT_BOOLEAN_VALUE(PROCESSOR_KNOWN))
    return 3;

  if (!DETECT_BOOLEAN_VALUE(LIBC_GLIBC) ||
      !DETECT_BOOLEAN_VALUE(LIBC_MUSL) ||
      !DETECT_BOOLEAN_VALUE(LIBC_BIONIC) ||
      !DETECT_BOOLEAN_VALUE(LIBC_MSVC) ||
      !DETECT_BOOLEAN_VALUE(LIBC_APPLE) ||
      !DETECT_BOOLEAN_VALUE(LIBC_BSD) ||
      !DETECT_BOOLEAN_VALUE(LIBC_UCLIBC) ||
      !DETECT_BOOLEAN_VALUE(LIBC_NEWLIB) ||
      !DETECT_BOOLEAN_VALUE(LIBC_WASI) ||
      !DETECT_BOOLEAN_VALUE(LIBC_EMSCRIPTEN) ||
      !DETECT_BOOLEAN_VALUE(LIBC_KNOWN) ||
      !DETECT_BOOLEAN_VALUE(LIBRARY_KNOWN))
    return 4;

  /*
   * Deliberately no assertion that *_KNOWN must be true.
   * An otherwise-valid C11 target outside these known families is accepted.
   */
  return 0;
}
