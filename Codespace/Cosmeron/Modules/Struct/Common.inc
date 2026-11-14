#pragma once

#include "../../Core/Module/Module.h"
#include "../../Core/Outcome/Status.h"
#include "../../Core/Preprocessor/Compiling.inc"
#include "../../Core/Preprocessor/Detect.h"
#include "../../Core/Primitive/Types.h"

#include <complex.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ==========================================================================
 * Module Template Forwarding Helper
 * Enables: TEMPLATE_INSTANTIATE(Struct, Target, Params...)
 * ========================================================================== */

#ifndef TEMPLATE_Struct
#define TEMPLATE_Struct(TARGET, ...) NS(TEMPLATE_Struct, TARGET)(__VA_ARGS__)
#endif

/* EOF */
