#pragma once
/*
 * BSD ELF Clang/lld can honor #pragma comment(lib, ...) on object files.
 *
 * This is an OPTIONAL implicit link of the system pthread library. It keeps
 * OS threading intact and avoids requiring users to write -pthread when the
 * toolchain supports ELF autolinking. It does NOT meet the stricter policy of
 * having no external link dependency at all. GCC/GNU ld are unaffected.
 *
 * Included only by native-threaded headers, not Cooperative.h or Atomic.h.
 */
#if (defined(__FreeBSD__) || defined(__OpenBSD__) || \
     defined(__NetBSD__) || defined(__DragonFly__)) && \
    defined(__clang__) && !defined(COSMERON_NO_IMPLICIT_LINK)
#pragma comment(lib, "pthread")
#endif
