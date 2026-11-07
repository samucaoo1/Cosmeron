#pragma once

#include "Type.space"

/* -------------------------------------------------------------------------- */
/* Naming Styles                                                              */
/* -------------------------------------------------------------------------- */

#if defined(TYPE_ALIAS_SMALL) && defined(TYPE_ALIAS_COMPLETE)
#error "TYPE_ALIAS_SMALL and TYPE_ALIAS_COMPLETE are mutually exclusive"
#endif

#if !defined(TYPE_ALIAS_SMALL) && !defined(TYPE_ALIAS_COMPLETE)
#define TYPE_ALIAS_COMPLETE
#endif

#if defined(TYPE_ALIAS_SMALL)
#define TYPE_ALIAS_SELECT(SMALL, COMPLETE) SMALL
#else
#define TYPE_ALIAS_SELECT(SMALL, COMPLETE) COMPLETE
#endif

#define TYPE_ALIAS(SMALL, COMPLETE)                                           \
  TYPE_NS(TYPE_ALIAS_SELECT(SMALL, COMPLETE))

/* -------------------------------------------------------------------------- */
/* Type Aliases Emitted via X-Macro Row-Tuple Pattern                         */
/* -------------------------------------------------------------------------- */

#define FUNDAMENTAL_TYPEDEF_EMIT(ROW)                                          \
  typedef FUNDAMENTAL_TYPE_GET(TYPE, ROW)                                      \
      TYPE_ALIAS(FUNDAMENTAL_TYPE_GET(SMALL, ROW),                             \
                 FUNDAMENTAL_TYPE_GET(COMPLETE, ROW));

FUNDAMENTAL_TYPE_TABLE(FUNDAMENTAL_TYPEDEF_EMIT)

#undef FUNDAMENTAL_TYPEDEF_EMIT
/* EOF */
