#pragma once

#include "Call.space"

#include <stdbool.h>
#include <stddef.h>

/*
 * Call represents a typed function together with a non-owning context.
 *
 * Declare one callable type per function signature:
 *
 *   CALL_DECLARE(Transform, int, (int value), (value))
 *
 * The generated function receives the declared arguments followed by
 * void *context. The generated Invoke function accepts the callable and
 * the declared arguments, then forwards its stored context.
 *
 * The context is never copied, allocated, or released by Call. The caller
 * must keep it alive for as long as the callable may be invoked.
 */

#include "Impl/Call.impl"
/* EOF */
