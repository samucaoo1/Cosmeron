# Embedded SQLite and zero-link policy

Cosmeron embeds SQLite as one vendor implementation translation unit:
define `DATABASE_SQLITE_IMPLEMENTATION` in one C file (e.g. the supplied
`Codespace/Tests/Database/Backend.c`), then include
`Cosmeron/Modules/Database/SQLite/SQLite.h`. No external SQLite library
is linked.

## Threading choice

| Target | Default `DATABASE_SQLITE_THREADSAFE` | Library link behavior |
| --- | --- | --- |
| FreeBSD / OpenBSD / NetBSD / DragonFly | `0` | SQLite mutex-free backend, intended to avoid `-pthread` |
| Linux / macOS / Windows | `1` | Existing SQLite serialized/thread-safe backend retained |
| Explicit opt-in on BSD | `1` | Thread-safe SQLite; external `-pthread` may be required |

`DATABASE_SQLITE_THREADSAFE=0` **disables SQLite's mutex layer**.
Do **not** call SQLite simultaneously from different threads, even if using
different database connections. A serial event loop or single database
worker is an appropriate use. Do not interpret the presence of Cosmeron's
Concurrency module as permission to access SQLite concurrently under
this mode. This changes the guarantees of the **embedded SQLite build**,
not of the SQL file format.

To opt back into SQLite's internal mutexes, define the macro to `1`
before including the implementation header, and supply OS-specific
linking flags if the toolchain requires them:

```c
#define DATABASE_SQLITE_THREADSAFE 1
#define DATABASE_SQLITE_IMPLEMENTATION
#include "Cosmeron/Modules/Database/SQLite/SQLite.h"
```

Selection is compile-time, application-wide and must be identical to the
one implementation object. Do not build two vendor implementations with
conflicting settings. The `SQLiteThreadMode.c` regression calls the
embedded SQLite `sqlite3_threadsafe()` entry point to confirm the
compiled mode; persistence and multi-TU tests check the public API.

**Caveat:** avoiding an explicit linker flag is not equivalent to removing
all runtime OS dependencies. Other backends (Audio, Concurrency) may still
require native shared libraries on BSD.
