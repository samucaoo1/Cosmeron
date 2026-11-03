#pragma once

#include "../../Preprocessor/Compiling.inc"
#include "../../Preprocessor/Operations.h"

#include "Category.impl"
#include "Generic.impl"
#include "Memory.impl"
#include "Concurrency.impl"
#include "Arithmetic.impl"
#include "Sequence.impl"

#define STATUS_COL_NAME(NAME, CATEGORY, MESSAGE) NAME
#define STATUS_COL_CATEGORY(NAME, CATEGORY, MESSAGE) CATEGORY
#define STATUS_COL_MESSAGE(NAME, CATEGORY, MESSAGE) MESSAGE

#define STATUS_GET(FIELD, ROW) PP_OP_CAT2(STATUS_COL_, FIELD) ROW

#define STATUS_TABLE(X)                                                        \
  STATUS_TABLE_GENERIC(X)                                                      \
  STATUS_TABLE_MEMORY(X)                                                       \
  STATUS_TABLE_CONCURRENCY(X)                                                  \
  STATUS_TABLE_ARITHMETIC(X)                                                   \
  STATUS_TABLE_SEQUENCE(X)
/* EOF */
