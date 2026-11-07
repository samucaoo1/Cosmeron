#pragma once

#include "../Module/Namespace.h"
#include "../Outcome/Status.h"

#define DIAGNOSTIC_TYPE(SUFFIX) PP_OP_CAT3(T, Diagnostic, SUFFIX)
#define DIAGNOSTIC_INS(NAME) NS(LIB_PREFIX(Diagnostic), Internal, NAME)
#define DIAGNOSTIC_CINS(NAME) CNS(LIB_PREFIX_CONST(DIAGNOSTIC), INTERNAL, NAME)

