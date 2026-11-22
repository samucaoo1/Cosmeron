#pragma once

#include "../Namespace.h"
#include "../Common/Tuple.impl"

#include <stdbool.h>
#include <stddef.h>

#define TCALLABLE(NAME) LIB_PREFIX(NS_JOIN(TCallable, NAME))
#define CALLABLE_FUNC(NAME) LIB_PREFIX(NS_JOIN(Callable, NAME))
#define CALLABLE_EMPTY NULL

#include "Impl/Callable.impl"
/* EOF */
