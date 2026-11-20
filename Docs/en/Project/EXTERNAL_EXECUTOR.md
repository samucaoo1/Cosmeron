# External Executor contract (C11)

Include `Cosmeron/Modules/Concurrency/Task/Executor.h` or
`Concurrency/Cooperative.h`. An executor is a `submit(userData, TTask)`
callback and context. `Executor_Submit` performs **no** allocation and
does not spin up threads. The host may schedule tasks with pthread, SDL,
another runtime, or synchronously.

`TTask` is passed **by value**. A deferred executor must copy it into its
own queue, and the caller must guarantee `task.argument` stays alive until
execution ends; scheduling does not transfer ownership of the argument.
Shared memory synchronization and executor shutdown are the host's
responsibility. `Executor_RunInline` invokes a task immediately, without
real concurrency.

This is an integration extension, **not** a substitute for full thread
creation or mutexes and not proof that native threaded backends satisfy
zero-extra-link on BSD.
