#include "../../Cosmeron/Modules/Database/Statement/Statement.h"
OPSTATUS readDatabase(DATABASE_TYPE(TStatement) *statement, int64_t *outValue) {
  bool row;
  OPSTATUS status = DATABASE_FUNC(Statement_Next)(statement, &row);
  if (status != STATUS_CONST(SUCCESS) || !row) return STATUS_CONST(GENERIC_ERROR);
  return DATABASE_FUNC(Statement_ColumnInt64)(statement, 0, outValue);
}
