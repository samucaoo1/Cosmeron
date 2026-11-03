#pragma once

#include "../Module.space"
#include "../Common/Tuple.impl"

#include <stdbool.h>
#include <stddef.h>

#define TCALLBACK(NAME) GNS2(TCallback, NAME)
#define CALLBACK_FUNC(NAME) GNS2(Callback, NAME)

#include "Impl/Callback.impl"
/* EOF */
