#include "../../Cosmeron/Core/Preprocessor/Detect/Compiler.h"
#include "../../Cosmeron/Core/Preprocessor/Detect/OperationSystem.h"
#include "../../Cosmeron/Core/Preprocessor/Detect/Processor.h"

#if (COMPILER_CLANG + COMPILER_GCC + COMPILER_MSVC + COMPILER_INTEL) > 1
#error "Primary compiler-family detection must be mutually exclusive."
#endif

#if (OS_WINDOWS + OS_MAC + OS_LINUX + OS_FREEBSD) > 1
#error "Operating-system detection must identify at most one supported target."
#endif

#if (PROCESSOR_X86 + PROCESSOR_X64 + PROCESSOR_ARM + PROCESSOR_ARM64 +        \
     PROCESSOR_RISCV) > 1
#error "Processor detection must identify at most one architecture."
#endif

#if COMPILER_KNOWN !=                                                        \
    (COMPILER_CLANG || COMPILER_GCC || COMPILER_MSVC || COMPILER_INTEL)
#error "COMPILER_KNOWN must describe, not constrain, compiler detection."
#endif

#if OS_KNOWN != (OS_WINDOWS || OS_MAC || OS_LINUX || OS_FREEBSD)
#error "OS_KNOWN must describe, not constrain, operating-system detection."
#endif

#if PROCESSOR_KNOWN !=                                                       \
    (PROCESSOR_X86 || PROCESSOR_X64 || PROCESSOR_ARM || PROCESSOR_ARM64 ||   \
     PROCESSOR_RISCV)
#error "PROCESSOR_KNOWN must describe, not constrain, processor detection."
#endif

#if OS_UNIX != (OS_LINUX || OS_MAC || OS_FREEBSD)
#error "OS_UNIX classification is inconsistent."
#endif

#if OS_POSIX != (OS_LINUX || OS_MAC || OS_FREEBSD)
#error "OS_POSIX classification is inconsistent."
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
      !DETECT_BOOLEAN_VALUE(OS_LINUX) || !DETECT_BOOLEAN_VALUE(OS_FREEBSD) ||
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

  /*
   * Deliberately no assertion that *_KNOWN must be true.
   * An otherwise-valid C11 target outside these known families is accepted.
   */
  return 0;
}
