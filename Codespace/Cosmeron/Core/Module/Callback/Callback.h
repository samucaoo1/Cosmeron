#pragma once

#include "../Module.space"
#include "../Common/Tuple.impl"

#include <stdbool.h>
#include <stddef.h>

#define TCALLBACK(NAME) NS(TCallback, NAME)
#define CALLBACK_FUNC(NAME) NS(Callback, NAME)

#include "Impl/Callback.impl"
/* EOF */
