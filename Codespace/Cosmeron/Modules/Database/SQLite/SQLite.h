#pragma once

/* Define DATABASE_SQLITE_IMPLEMENTATION before the FIRST project/system header
 * in exactly one translation unit. All other TUs include declarations only.
 * The fixed vendor namespace is independent of the public Cosmeron namespace. */
/*
 * The embedded amalgamation is compiled exactly once. On BSD, SQLite's
 * mutex backend references libpthread, which conflicts with the public
 * zero-extra-link contract. Default to SQLite single-thread mode there.
 *
 * WARNING: DATABASE_SQLITE_THREADSAFE=0 disables SQLite mutexes globally.
 * Do not use SQLite from multiple threads simultaneously, even through
 * distinct connections, in this configuration.
 *
 * Consumers needing SQLite's serialized/thread-safe behavior may explicitly
 * define DATABASE_SQLITE_THREADSAFE=1 before the IMPLEMENTATION TU; on BSD
 * this can require -pthread and therefore opts out of zero-link.
 *
 * The selected mode is an application-wide build decision, not a per-
 * connection or runtime toggle. Linux/Windows/macOS remain thread-safe.
 */
#ifndef DATABASE_SQLITE_THREADSAFE
#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || \
    defined(__DragonFly__)
#define DATABASE_SQLITE_THREADSAFE 0
#else
#define DATABASE_SQLITE_THREADSAFE 1
#endif
#endif
#if DATABASE_SQLITE_THREADSAFE != 0 && DATABASE_SQLITE_THREADSAFE != 1
#error "DATABASE_SQLITE_THREADSAFE must be exactly 0 or 1"
#endif

#ifdef DATABASE_SQLITE_IMPLEMENTATION
#define cosmeron_database_vendor_SQLITE_THREADSAFE DATABASE_SQLITE_THREADSAFE
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
