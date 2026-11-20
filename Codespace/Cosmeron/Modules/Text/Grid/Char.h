#pragma once

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Memory/Alloc.h"
#include "../../../Core/Module/Module.h"
#include "../../Tuple/Dual.h"
#include "../../Tuple/Quad.h"
#include "../Impl/Common.inc"
#include "Char.def"
#include "Impl/Char.impl"

TEMPLATE_INSTANTIATE(Text, Char, Char8, MODULE_TYPE(Text, Char8));
TEMPLATE_INSTANTIATE(Text, Char, Char16, MODULE_TYPE(Text, Char16));
TEMPLATE_INSTANTIATE(Text, Char, Char32, MODULE_TYPE(Text, Char32));

/* EOF */
