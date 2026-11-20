#pragma once

#include "../Impl/Common.inc"
#include "../Value/Clamp.h"
#include "Basic.h"
#include "Clamped.def"

TEMPLATE_INSTANTIATE_TABLE(Math, ClampedArithmeticSigned,
                           MATH_SIGNED_INT_TABLE);
TEMPLATE_INSTANTIATE_TABLE(Math, ClampedArithmeticUnsigned,
                           MATH_UNSIGNED_INT_TABLE);

/* EOF */
