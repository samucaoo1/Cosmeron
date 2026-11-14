#pragma once

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Memory/Alloc.h"
#include "../../../Core/Module/Module.h"
#include "../../Tuple/TDual.h"
#include "../../Tuple/TQuad.h"
#include "../Impl/Common.inc"
#include "Impl/Char.impl"
#include "Char.def"

TEMPLATE_INSTANTIATE(Text_Char, Char8, MODULE_TYPE(Text, Char8));
TEMPLATE_INSTANTIATE(Text_Char, Char16, MODULE_TYPE(Text, Char16));
TEMPLATE_INSTANTIATE(Text_Char, Char32, MODULE_TYPE(Text, Char32));
