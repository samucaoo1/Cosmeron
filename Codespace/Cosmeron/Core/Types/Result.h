#pragma once

#include "../Condition/Status.h"
#include "../Condition/Error.h"
#include "../Module/Callback/Callback.h"
#include "../Module/Module.h"
#include "../Preprocessor/Compiling.inc"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* ========================================================================== */
/* Type Module: Declarative Template Object Result                         */
/* ========================================================================== */

#ifndef TEMPLATE_Type
#define TEMPLATE_Type(TARGET, ...) NS(TEMPLATE_Type, TARGET)(__VA_ARGS__)
#endif

#include "Result.def"

/* EOF */
