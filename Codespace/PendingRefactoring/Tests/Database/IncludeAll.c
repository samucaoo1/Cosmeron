#include "../../Cosmeron/Modules/Database/Statement/Statement.h"
#include "../../Cosmeron/Modules/Database/Transaction/Transaction.h"
#include "../../Cosmeron/Modules/Database/Database.h"
#include <assert.h>
int main(void) {
  DATABASE_TYPE(TDatabase) database = {0};
  assert(DATABASE_FUNC(Open)(&database, ":memory:") == STATUS_CONST(SUCCESS));
  assert(DATABASE_FUNC(Close)(&database) == STATUS_CONST(SUCCESS));
  return 0;
}
