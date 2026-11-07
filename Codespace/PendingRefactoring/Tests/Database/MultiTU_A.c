#include "../../Cosmeron/Modules/Database/Database.h"
OPSTATUS openDatabase(DATABASE_TYPE(TDatabase) *database,
    DATABASE_TYPE(TStatement) *statement) {
  OPSTATUS status = DATABASE_FUNC(Open)(database, ":memory:");
  if (status == STATUS_CONST(SUCCESS)) status = DATABASE_FUNC(Prepare)(database, "SELECT 123", statement);
  return status;
}
