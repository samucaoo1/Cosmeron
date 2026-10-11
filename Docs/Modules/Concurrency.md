# Concurrency

The **Concurrency** module provides native threads, synchronization primitives, atomics, task execution, futures, and cooperative coroutines. This document maps the **implemented public API** in the `audio-module` branch. Native backend availability and operational semantics must be checked for the target platform.

```c
/* Native threads, synchronization and task-pool facilities */
#include "Cosmeron/Modules/Concurrency/Concurrency.h"

/* Cooperative facilities without native thread requirements */
#include "Cosmeron/Modules/Concurrency/Cooperative.h"
```

---

# Overview

`Concurrency.h` is a compatibility umbrella that includes `Native.h`. For a cooperative, single-threaded path, include `Cooperative.h` instead; do not assume this runs background tasks.

| Package | Header | Main concepts |
| --- | --- | --- |
| Atomic | `Atomic/Atomic.h` | atomic U32, U64, Ptr and Bool |
| Coroutine | `Coroutine/Coroutine.h` | explicit resume/yield state machines |
| Thread | `Thread/Thread.h`, `Thread/Sleep.h` | OS threads, lifecycle, sleep |
| Synchronization | `Synchronization/*.h` | mutex, RWLock, condition, semaphore, barrier, once |
| Task | `Task/Task.h` | task descriptions and synchronous invocation |
| Executor | `Task/Executor.h` | submission abstraction and inline execution |
| Future | `Task/Future.h` | completion/result synchronization |
| ThreadPool | `Task/ThreadPool.h` | queued native workers |

# Naming and shared types

All public names are qualified through `CONCURRENCY_TYPE`, `CONCURRENCY_FUNC` or specialized package macros. Use namespace macros rather than hardcoding default-prefix C symbols. Most operations return `OPSTATUS`; queries may instead return `bool`, and atomic operations return their value type.

Native resource objects require the corresponding initialization and release calls. Do not copy active native handles or synchronization objects by value. For tasks/futures, track the ownership of any user-provided argument and result separately.

# API reference

## Function summary

The list below names actual declarations; `Foo` under a package means its package's `*_FUNC(Foo)` name. The precise signatures reside in the linked header.

| Package | Operations | Result family |
| --- | --- | --- |
| Atomic U32, U64 | Load, Store, Exchange, CompareExchange, FetchAdd, FetchSub, FetchAnd, FetchOr, FetchXor | value, bool or void |
| Atomic Ptr | Load, Store, Exchange, CompareExchange | value, bool or void |
| Atomic Bool | Load, Store, Exchange | bool or void |
| Coroutine | Init, Resume, Reset, GetState, IsReady, IsRunning, IsSuspended, IsFinished | OPSTATUS or bool |
| Thread | Create, Join, Detach, Yield, Current, Equal, SleepFor, SleepUntil | OPSTATUS, void, handle or bool |
| Mutex | Init, Destroy, Lock, TryLock, Unlock | OPSTATUS |
| RWLock | Init, Destroy, ReadLock, TryReadLock, ReadUnlock, WriteLock, TryWriteLock, WriteUnlock | OPSTATUS |
| Condition | Init, Destroy, Wait, NotifyOne, NotifyAll | OPSTATUS |
| Semaphore | Init, Destroy, Wait, TryWait, Post | OPSTATUS |
| Barrier | Init, Destroy, Wait | OPSTATUS |
| Once | Call | OPSTATUS |
| Task | Create, IsValid, Run | task, bool or OPSTATUS |
| Executor | Submit, RunInline | OPSTATUS |
| Future | Init, Destroy, IsReady, Complete, Wait, Get, Task | bool or OPSTATUS |
| ThreadPool | Init, Submit, TrySubmit, Shutdown, Destroy | OPSTATUS |

# Coroutine package

Cooperative coroutines are explicit state machines: `Resume` advances until they yield or finish. They are not automatically scheduled OS threads.

## Resume

### Syntax

#### Macro form

```c
OPSTATUS COROUTINE_FUNC(Resume)(CONCURRENCY_TYPE(TCoroutine) *coroutine);
```

#### Direct form

`OPSTATUS Concurrency_Coroutine_Resume(Concurrency_TCoroutine *coroutine);` with the default prefix. Use the macro for portable prefix configuration.

### Parameters

The initialized coroutine state to resume.

### Return value

`OPSTATUS`, with the coroutine's lifecycle state queried separately.

### Remarks

The implementation uses `COROUTINE_BEGIN`, `COROUTINE_YIELD`, and `COROUTINE_END` continuation macros. Such a coroutine should not be confused with a stackful fiber or a preemptive thread.

### Example

```c
/* See Coroutine/Coroutine.h for the matching state and callback type. */
COROUTINE_FUNC(Resume)(&coroutine);
```

# Atomic package

The public atomic types include `CONCURRENCY_TYPE(TAtomicU32)`, `TAtomicU64`, `TAtomicPtr`, and `TAtomicBool`. Initializer macros are `ATOMIC_U32_INIT`, `ATOMIC_U64_INIT`, `ATOMIC_PTR_INIT`, and `ATOMIC_BOOL_INIT`.

## U32_Load

### Syntax

#### Macro form

```c
uint32_t ATOMIC_FUNC(U32_Load)(const CONCURRENCY_TYPE(TAtomicU32) *atomic, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

`uint32_t Concurrency_Atomic_U32_Load(const Concurrency_TAtomicU32 *atomic, Concurrency_MemoryOrder order);` in the default namespace.

### Parameters

A pointer to the corresponding atomic object and the memory-order argument defined in the header.

### Return value

A `uint32_t` snapshot of the atomic value.

### Remarks

Load alone does not perform a compound compare/update. Consult the documented memory-order enum in `Atomic.h`; valid ordering combinations differ among load, store, and compare-exchange operations.

### Example

```c
/* Inspect Atomic.h for the target's supported memory-order constants. */
```

# Native synchronization

## Mutex Lock

### Syntax

#### Macro form

```c
OPSTATUS MUTEX_FUNC(Lock)(CONCURRENCY_TYPE(TMutex) *mutex);
```

#### Direct form

`OPSTATUS Concurrency_Mutex_Lock(Concurrency_TMutex *mutex);` in the default namespace.

### Parameters

A pointer to an initialized `CONCURRENCY_TYPE(TMutex)`.

### Return value

`OPSTATUS`.

### Remarks

Pair locking with `MUTEX_FUNC(Unlock)`; release the native resource with `MUTEX_FUNC(Destroy)` only after all users finish. `TryLock` additionally writes an acquisition outcome to an out parameter.

### Example

```c
CONCURRENCY_TYPE(TMutex) mutex;
if (MUTEX_FUNC(Init)(&mutex) == STATUS_CONST(SUCCESS)) {
    /* Check Lock status before accessing shared data. */
    MUTEX_FUNC(Lock)(&mutex);
    MUTEX_FUNC(Unlock)(&mutex);
    MUTEX_FUNC(Destroy)(&mutex);
}
```

# Thread and task lifecycle

Native threads expose `Create`, `Join`, and `Detach`. `Join` or `Detach` manages the thread lifecycle; neither means force-stopping an executing callback. A task is a function/argument pair; calling `Task_Run` executes it directly. An executor may run tasks inline or delegate submission. A thread pool schedules queued work on workers and must be shut down and destroyed according to its API.

# Futures

A future has explicit initialization, completion, readiness checking, waiting, result extraction, and destruction. The callback type `CONCURRENCY_TYPE(FutureFunction)` returns `OPSTATUS` and writes a `void *` result through an output pointer. The application must define ownership and lifetime for pointed-to results.

# Contracts and limitations

- C11, header-only declarations; backend support is platform-dependent.
- Native code may use Win32 or POSIX synchronization APIs. Test each supported target rather than treating all platforms as interchangeable.
- Cooperative coroutines do not require native threads and do not imply parallel execution.
- `TryLock`, `TryWait`, and `TrySubmit` have separate status/outcome behavior; check the return status and any output flag.
- This initial reference indexes the complete public *operation families*, but not all parameter and return contracts have yet been expanded into individual function entries. Treat it as the first documentation pass, not a claim of Bit-style completeness.

# See also

- [Chronometry](Chronometry.md) — duration and clock types used by sleep/timed operations.
- [Error](Error.md) — `OPSTATUS` return conventions.
- [Algorithm](Algorithm.md) — comparison and callback conventions.
- [Bit](Bit.md) — target reference-document format.
