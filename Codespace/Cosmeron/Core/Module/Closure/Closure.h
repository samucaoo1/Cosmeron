#pragma once

#include "../Namespace.h"
#include "../Common/Tuple.impl"
#include "../Callback/Callback.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define TCLOSURE(NAME) LIB_PREFIX(NS_JOIN(TClosure, NAME))
#define CLOSURE_FUNC(NAME) LIB_PREFIX(NS_JOIN(Closure, NAME))

#include "Impl/Closure.impl"
/* EOF */

