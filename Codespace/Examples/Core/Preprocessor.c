#include "../../Cosmeron/Core/Preprocessor/Detect/Detect.h"

#include <stdio.h>

int main(void) {
  printf("Platform detection via Detect.h:\n");
  printf("  OS_LINUX=%d OS_WINDOWS=%d OS_MAC=%d OS_POSIX=%d\n", OS_LINUX,
         OS_WINDOWS, OS_MAC, OS_POSIX);
  printf("  COMPILER_GCC=%d COMPILER_CLANG=%d COMPILER_MSVC=%d\n",
         COMPILER_GCC, COMPILER_CLANG, COMPILER_MSVC);
  printf("  PROCESSOR_X64=%d PROCESSOR_ARM64=%d PROCESSOR_WASM=%d\n",
         PROCESSOR_X64, PROCESSOR_ARM64, PROCESSOR_WASM);
  return 0;
}
