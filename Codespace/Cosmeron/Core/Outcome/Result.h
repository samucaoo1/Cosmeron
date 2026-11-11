#pragma once

#include "Status.h"
#include "../Condition/Error.h"
#include "../Module/Callback/Callback.h"
#include "../Module/Module.h"
#include "../Preprocessor/Compiling.inc"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* ========================================================================== */
/* Outcome Module: Declarative Template Object Result                         */
/* ========================================================================== */

#ifndef TEMPLATE_Outcome
#define TEMPLATE_Outcome(TARGET, ...) NS(TEMPLATE_Outcome, TARGET)(__VA_ARGS__)
#endif

#include "Result.def"

/* EOF */
