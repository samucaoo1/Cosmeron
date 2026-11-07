#pragma once

#include "../Module/Namespace.h"
#include "../Preprocessor/Compiling.inc"
#include "StatusTable/StatusTable.h"

#define OPSTATUS MODULE_ENUM(Status, Status)

typedef enum MODULE_ENUM(Status, Category) {
#define STATUS_CATEGORY_ENUM_ITEM(ROW)                                         \
  CNS(LIB_PREFIX_CONST(STATUS), CATEGORY, STATUS_CATEGORY_GET(NAME, ROW)),
  STATUS_CATEGORY_TABLE(STATUS_CATEGORY_ENUM_ITEM)
#undef STATUS_CATEGORY_ENUM_ITEM
} MODULE_ENUM(Status, Category);

typedef enum MODULE_ENUM(Status, Status) {
#define STATUS_ENUM_ITEM(ROW) STATUS(STATUS_GET(NAME, ROW)),
  STATUS_TABLE(STATUS_ENUM_ITEM)
#undef STATUS_ENUM_ITEM
} OPSTATUS;

#ifndef STATUS_CATEGORY_CNS
#define STATUS_CATEGORY_CNS(NAME) CNS(LIB_PREFIX_CONST(STATUS), CATEGORY, NAME)
#endif
#ifndef STATUS_CATEGORY_CONST
#define STATUS_CATEGORY_CONST(NAME) STATUS_CATEGORY_CNS(NAME)
#endif
/* EOF */
