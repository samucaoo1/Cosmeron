#pragma once

#include "../Namespace.h"
#include "../Common/Tuple.impl"
#include "../Callable/Callable.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define TDELEGATE(NAME) LIB_PREFIX(NS_JOIN(TDelegate, NAME))
#define DELEGATE_FUNC(NAME) LIB_PREFIX(NS_JOIN(Delegate, NAME))

#include "Impl/Delegate.impl"
/* EOF */
