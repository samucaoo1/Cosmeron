#pragma once

#include "../../../Core/Module/Module.h"
#include "Attribute.h"
#include "Char.h"
#include "Grid.def"

TEMPLATE_INSTANTIATE(Text, Grid, Char8, MODULE_TYPE(Text, Char8));
TEMPLATE_INSTANTIATE(Text, Grid, Char16, MODULE_TYPE(Text, Char16));
TEMPLATE_INSTANTIATE(Text, Grid, Char32, MODULE_TYPE(Text, Char32));

/* EOF */
