#pragma once
#if !defined(_WIN32)
#include <dlfcn.h>
/* POSIX dlsym represents function addresses as void pointers. memcpy avoids
 * ISO C object-pointer/function-pointer cast diagnostics. */
#define COSMERON_INPUT_DLSYM(LIBRARY, TARGET, NAME) do { \
  void *address = dlsym((LIBRARY), (NAME)); \
  _Static_assert(sizeof(address) == sizeof(TARGET), "POSIX function pointer size"); \
  memcpy(&(TARGET), &address, sizeof(address)); \
} while (0)
#endif
