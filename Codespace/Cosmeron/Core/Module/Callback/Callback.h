#pragma once

#include "../Namespace.h"
#include "../Common/Tuple.impl"

#include <stdbool.h>
#include <stddef.h>

#define TCALLBACK(NAME) LIB_PREFIX(NS_JOIN(TCallback, NAME))
#define CALLBACK_FUNC(NAME) LIB_PREFIX(NS_JOIN(Callback, NAME))
#define CALLBACK_EMPTY NULL

#include "Impl/Callback.impl"
/* EOF */
