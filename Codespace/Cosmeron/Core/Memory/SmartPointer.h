#pragma once

#include "../Outcome/Status.h"
#include "../Module/Module.h"
#include "Alloc.h"
#include "Common.inc"
#include "Internal/SmartPointer_Common.inc"

/* ========================================================================== */
/* Memory Smart Pointers Template Include & Forwarding Engines                */
/* ========================================================================== */

#include "UniquePtr.def"
#include "WeakPtr.def"
#include "SharedPtr.def"
#include "WeakPtrAtomic.def"
#include "SharedPtrAtomic.def"
#include "IntrusivePtr.def"

/* EOF */
