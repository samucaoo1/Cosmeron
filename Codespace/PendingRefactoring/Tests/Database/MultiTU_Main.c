#include "../../Cosmeron/Modules/Database/Database.h"
#include <assert.h>
OPSTATUS openDatabase(DATABASE_TYPE(TDatabase) *, DATABASE_TYPE(TStatement) *);
OPSTATUS readDatabase(DATABASE_TYPE(TStatement) *, int64_t *);
int main(void) {
  DATABASE_TYPE(TDatabase) database = {0};
  DATABASE_TYPE(TStatement) statement = {0};
  int64_t value;
  assert(openDatabase(&database, &statement) == STATUS_CONST(SUCCESS));
  assert(readDatabase(&statement, &value) == STATUS_CONST(SUCCESS) && value == 123);
  assert(DATABASE_FUNC(Statement_Close)(&statement) == STATUS_CONST(SUCCESS));
  assert(DATABASE_FUNC(Close)(&database) == STATUS_CONST(SUCCESS));
  return 0;
}
