#pragma once

#include "../../Error/Status.h"
#include "../Package.h"

#include <stdbool.h>
#include <stddef.h>

#define TCALLBACK(NAME) GNS2(TCallback, NAME)
#define CALLBACK_FUNC(NAME) GNS2(Callback, NAME)

#include "Impl/Callback.impl"
/* EOF */
