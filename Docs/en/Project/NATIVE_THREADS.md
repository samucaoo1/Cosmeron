# Native Concurrency and BSD linker policy

`Concurrency/Cooperative.h` exposes atomics, coroutines and directly
executed tasks without OS thread creation. It does **not** provide
parallelism, joins, mutexes or a thread pool.

`Concurrency/Native.h` exposes the full existing native Thread, Mutex,
RWLock, Semaphore, Condition, Barrier, Future and ThreadPool functions.
The POSIX implementation currently uses `pthread_*` and may require
`-pthread` or a platform-specific runtime. Windows uses its native APIs.

`Concurrency/Concurrency.h` remains a compatible umbrella for the native
profile. These headers intentionally preserve user-visible function names
and types. We do **not** silently replace mutexes with atomics or pretend
that cooperative tasks have parallel execution semantics.

CI performs three separate checks on FreeBSD, OpenBSD, NetBSD and
DragonFly BSD:

1. **Cooperative** — must link and run without extra user linker flags.
2. **Native pthread** — attempts a strict zero-extra-link probe; if it
   fails, runs real functionality with the required native flags, recording
   the zero-link gap explicitly.
3. **C11 thrd_create/thrd_join** — a separate *actual thread* probe with
   no `-pthread` or `-lstdthreads`; its result indicates feasibility,
   but cannot certify drop-in equivalence for the existing pthread-based
   synchronization types.

An alternative native backend must preserve joins, detach semantics,
synchronization and multi-TU safety under contention. Replacing symbol
names or using `dlopen` does not alone prove true zero-link portability.
