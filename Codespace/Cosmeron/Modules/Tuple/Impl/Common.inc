#pragma once

#include "../../../Core/Module/Module.h"
#include "../../../Core/Condition/Status.h"
#include "../../../Core/Preprocessor/Compiling.inc"
#include "../../../Core/Preprocessor/Detect/Detect.h"
#include "../../../Core/Types/Primitive.h"

#include <complex.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ==========================================================================
 * Module Template Forwarding Helper
 * Enables: TEMPLATE_INSTANTIATE(Tuple, Target, Params...)
 * ========================================================================== */

#ifndef TEMPLATE_Tuple
#define TEMPLATE_Tuple(TARGET, ...) NS(TEMPLATE_Tuple, TARGET)(__VA_ARGS__)
#endif

/* EOF */
