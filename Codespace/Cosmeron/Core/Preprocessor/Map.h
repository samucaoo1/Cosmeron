#pragma once

#include "Compiling.inc"
#include "Foreach.h"

#define PP_MAP(macro, ...) PP_FOREACH(macro, __VA_ARGS__)
/* EOF */
