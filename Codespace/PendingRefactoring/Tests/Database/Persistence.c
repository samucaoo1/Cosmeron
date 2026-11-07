#include "../../Cosmeron/Modules/Database/Database.h"
#include <assert.h>
#include <stdio.h>

#define OK(OPERATION) assert((OPERATION) == STATUS_CONST(SUCCESS))

static int64_t count(DATABASE_TYPE(TDatabase) *database) {
  DATABASE_TYPE(TStatement) statement = {0};
  bool row;
  int64_t value;
  OK(DATABASE_FUNC(Prepare)(database, "SELECT count(*) FROM records", &statement));
  OK(DATABASE_FUNC(Statement_Next)(&statement, &row)); assert(row);
  OK(DATABASE_FUNC(Statement_ColumnInt64)(&statement, 0, &value));
  OK(DATABASE_FUNC(Statement_Close)(&statement));
  return value;
}
int main(void) {
  const char *path = "database-persistence-test.db";
  DATABASE_TYPE(TDatabase) first = {0}, second = {0};
  DATABASE_TYPE(TConfig) config = DATABASE_FUNC(Config)(path);
  DATABASE_TYPE(TDiagnostic) diagnostic = {0};
  FILE *existing = fopen(path, "rb");
  assert(!existing); /* Never overwrite an existing user file. */
  config.flags = 0;
  assert(DATABASE_FUNC(OpenWith)(&first, &config) == STATUS_CONST(INVALID_ARGUMENT));
  config.flags = DATABASE_CONST(OPEN_READ_ONLY) | DATABASE_CONST(OPEN_CREATE);
  assert(DATABASE_FUNC(OpenWith)(&first, &config) == STATUS_CONST(INVALID_ARGUMENT));
  config.flags = UINT32_MAX;
  assert(DATABASE_FUNC(OpenWith)(&first, &config) == STATUS_CONST(INVALID_ARGUMENT));
  config = DATABASE_FUNC(Config)(path);
  config.backend = (DATABASE_TYPE(Backend))INT_MAX;
  assert(DATABASE_FUNC(OpenWith)(&first, &config) == STATUS_CONST(NOT_SUPPORTED));
  config = DATABASE_FUNC(Config)(path);
  config.timeoutMs = UINT32_MAX;
  assert(DATABASE_FUNC(OpenWith)(&first, &config) == STATUS_CONST(OUT_OF_RANGE));
  assert(!first.native);
  config.timeoutMs = 0;
  OK(DATABASE_FUNC(OpenWithDiagnostic)(&first, &config, &diagnostic));
  assert(!diagnostic.nativeCode);
  OK(DATABASE_FUNC(Execute)(&first, "CREATE TABLE records(id INTEGER PRIMARY KEY)"));
  assert(!DATABASE_FUNC(Transaction_IsActive)(&first));
  OK(DATABASE_FUNC(Transaction_Begin)(&first));
  assert(DATABASE_FUNC(Transaction_IsActive)(&first));
  OK(DATABASE_FUNC(Execute)(&first, "INSERT INTO records VALUES(1)"));
  OK(DATABASE_FUNC(Transaction_Savepoint)(&first, "\"; DELETE FROM records; --"));
  OK(DATABASE_FUNC(Execute)(&first, "INSERT INTO records VALUES(2)"));
  OK(DATABASE_FUNC(Transaction_RollbackTo)(&first, "\"; DELETE FROM records; --"));
  OK(DATABASE_FUNC(Transaction_Release)(&first, "\"; DELETE FROM records; --"));
  assert(count(&first) == 1);
  OK(DATABASE_FUNC(Transaction_Commit)(&first));
  OK(DATABASE_FUNC(Transaction_Begin)(&first));
  OK(DATABASE_FUNC(Execute)(&first, "INSERT INTO records VALUES(3)"));
  OK(DATABASE_FUNC(Transaction_Rollback)(&first));
  assert(count(&first) == 1);
  assert(DATABASE_FUNC(Transaction_Commit)(&first) != STATUS_CONST(SUCCESS));
  assert(DATABASE_FUNC(Transaction_Savepoint)(&first, "") == STATUS_CONST(INVALID_ARGUMENT));

  OK(DATABASE_FUNC(OpenWith)(&second, &config));
  OK(DATABASE_FUNC(Execute)(&first, "BEGIN IMMEDIATE"));
  assert(DATABASE_FUNC(ExecuteWithDiagnostic)(&second, "INSERT INTO records VALUES(4)", &diagnostic) == STATUS_CONST(BUSY));
  assert(diagnostic.nativeCode && *diagnostic.message);
  OK(DATABASE_FUNC(Transaction_Rollback)(&first));
  OK(DATABASE_FUNC(SetTimeout)(&second, 1));
  assert(DATABASE_FUNC(SetTimeout)(&second, UINT32_MAX) == STATUS_CONST(OUT_OF_RANGE));
  OK(DATABASE_FUNC(Execute)(&second, "INSERT INTO records VALUES(4)"));
  OK(DATABASE_FUNC(Close)(&second));

  OK(DATABASE_FUNC(Execute)(&first, "CREATE TABLE child(id INTEGER REFERENCES records(id))"));
  assert(DATABASE_FUNC(Execute)(&first, "INSERT INTO child VALUES(999)") != STATUS_CONST(SUCCESS));
  OK(DATABASE_FUNC(Close)(&first));
  config.flags = DATABASE_CONST(OPEN_READ_ONLY);
  OK(DATABASE_FUNC(OpenWith)(&first, &config));
  assert(count(&first) == 2);
  assert(DATABASE_FUNC(ExecuteWithDiagnostic)(&first, "DELETE FROM records", &diagnostic) != STATUS_CONST(SUCCESS));
  assert(diagnostic.nativeCode && *diagnostic.message);
  assert(count(&first) == 2);
  OK(DATABASE_FUNC(Close)(&first));
  assert(remove(path) == 0);
  assert(DATABASE_FUNC(OpenWithDiagnostic)(&first, &config, &diagnostic) != STATUS_CONST(SUCCESS));
  assert(!first.native && diagnostic.nativeCode && *diagnostic.message);
  assert(!DATABASE_FUNC(Transaction_IsActive)(&first));
  return 0;
}
