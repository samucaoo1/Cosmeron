#pragma once

/* Define DATABASE_SQLITE_IMPLEMENTATION before the FIRST project/system header
 * in exactly one translation unit. All other TUs include declarations only.
 * The fixed vendor namespace is independent of the public Cosmeron namespace. */
#ifdef DATABASE_SQLITE_IMPLEMENTATION
#define cosmeron_database_vendor_SQLITE_THREADSAFE 1
#define cosmeron_database_vendor_SQLITE_OMIT_LOAD_EXTENSION 1
#define cosmeron_database_vendor_SQLITE_DEFAULT_FOREIGN_KEYS 1
#define cosmeron_database_vendor_SQLITE_DQS 0
#define cosmeron_database_vendor_SQLITE_USE_URI 0
#define cosmeron_database_vendor_SQLITE_DEFAULT_MEMSTATUS 0
/* No optional SQL math functions: the embedded build needs no -lm. */
#if defined(_MSC_VER)
#pragma warning(push, 0)
#endif
#include "Internal/SQLite.inc"
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
#else
#include "Internal/Declarations.inc"
#endif
