#pragma once
/*
 * Cooperative/single-thread C11 subset: lock-free atomics, stackless
 * coroutines and directly executed tasks. No pthread or OS thread headers.
 *
 * This is NOT a drop-in replacement for Thread, Mutex or ThreadPool, and
 * does not claim real parallel execution. It can be included independently
 * of Concurrency.h on systems where linking native threads needs -pthread.
 */
#include "Concurrency.space"
#include "Atomic/Atomic.h"
#include "Coroutine/Coroutine.h"
#include "Task/Task.h"
