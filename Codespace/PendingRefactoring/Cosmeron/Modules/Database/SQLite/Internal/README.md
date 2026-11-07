# Embedded SQLite backend

SQLite **3.53.4**, Fossil source ID
`bf7c7f30031888f4e796e429ab3978879485813aaca6f641c7b33e4e09459bcc`.
The ID matches the official SQLite `version-3.53.4` source manifest:
<https://github.com/sqlite/sqlite/tree/version-3.53.4>.

The amalgamation was imported from the go-sqlite3 mirror at commit
`dbbe931904f2ee06546448fbe7cf7780f61bc110`:
<https://github.com/mattn/go-sqlite3/tree/dbbe931904f2ee06546448fbe7cf7780f61bc110>.
No Go runtime or Go adapter is included. SQLite's upstream public-domain
notices remain in the imported files; Cosmeron's adapters follow its own license.

Reproduce from the repository root:

```sh
python3 .github/scripts/vendor-database.py /path/to/pinned-go-sqlite3
```

The importer verifies both source SHA-256 hashes, removes only the mirror's
outer `USE_LIBSQLITE3` switch, and prefixes SQLite/FTS5 API identifiers, macros
and include guards. Comments, C string literals, OS symbol strings and copyright
notices are preserved. Escaped-newline literals are recognized. Imported
diagnostics are isolated from the strict authored-code checks.

## Inclusion

Define `DATABASE_SQLITE_IMPLEMENTATION` in exactly one C translation unit,
before its first system or project include. Include `SQLite/SQLite.h` or
`Database.h` there. Other translation units include the module normally.
The implementation resides in an included `.inc`; no sqlite library or
separately distributed SQLite `.c` compilation is needed.

This deliberately shares one engine, registry, allocator and mutex domain
across translation units. The engine's fixed private vendor prefix is separate
from configurable public Cosmeron namespaces. Do not call its private APIs.

## Configuration and platforms

The bundled build uses native mutexes (`THREADSAFE=1`, connections opened with
`FULLMUTEX`), foreign keys on by default, URI interpretation off, double-quoted
string literals off, and dynamic extension loading omitted. Optional SQL math
functions are not enabled. SQL, WAL, in-memory databases and normal file locking
are retained. Do not configure the bundled engine through an external SQLite
library: their global states are independent.

The zero-extra-link-flags targets are modern Linux/glibc >= 2.34, macOS and
Windows/MinGW/MSVC. SQLite uses pthread symbols on POSIX; on older glibc and some
BSD configurations these may require additional flags. Those targets are not
covered by the zero-link guarantee. Static libc linking is also outside that
guarantee. Mutexes are not disabled to disguise these dependencies.

Synchronize operations sharing a public connection/statement externally.
Native serialized SQLite calls do not make a sequence of public wrapper calls,
public handle mutation, or concurrent close safe.
