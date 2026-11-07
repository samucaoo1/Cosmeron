#pragma once
/*
 * Real OS-threaded Concurrency profile. It preserves the full existing
 * Thread/Mutex/ThreadPool API and its POSIX pthread / Windows backends.
 *
 * Unlike Cooperative.h, the POSIX backend can require -pthread on BSD.
 * Do not use this header to infer the zero-link contract is satisfied.
 */
#include "Concurrency.space"
#include "Atomic/Atomic.h"
#include "Thread/Thread.h"
#include "Thread/Sleep.h"
#include "Coroutine/Coroutine.h"
#include "Synchronization/Mutex.h"
#include "Synchronization/Once.h"
#include "Synchronization/RWLock.h"
#include "Synchronization/Condition.h"
#include "Synchronization/Semaphore.h"
#include "Synchronization/Barrier.h"
#include "Task/Task.h"
#include "Task/ThreadPool.h"
#include "Task/Future.h"
