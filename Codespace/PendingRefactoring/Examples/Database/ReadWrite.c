#define DATABASE_SQLITE_IMPLEMENTATION
#include "../../Cosmeron/Modules/Database/Database.h"
#include <stdio.h>

int main(void) {
  DATABASE_TYPE(TDatabase) database = {0};
  DATABASE_TYPE(TStatement) statement = {0};
  DATABASE_TYPE(TDiagnostic) diagnostic = {0};
  OPSTATUS status = DATABASE_FUNC(Open)(&database, ":memory:");
  bool hasRow;
  if (status != STATUS_CONST(SUCCESS)) goto cleanup;
  status = DATABASE_FUNC(ExecuteWithDiagnostic)(&database,
      "CREATE TABLE people(name TEXT NOT NULL, age INTEGER NOT NULL)", &diagnostic);
  if (status != STATUS_CONST(SUCCESS)) goto cleanup;
  status = DATABASE_FUNC(Prepare)(&database,
      "INSERT INTO people VALUES(?, ?)", &statement);
  if (status != STATUS_CONST(SUCCESS)) goto cleanup;
  status = DATABASE_FUNC(Statement_BindText)(&statement, 1, "Okarin", 6);
  if (status != STATUS_CONST(SUCCESS)) goto cleanup;
  status = DATABASE_FUNC(Statement_BindInt64)(&statement, 2, 18);
  if (status != STATUS_CONST(SUCCESS)) goto cleanup;
  status = DATABASE_FUNC(Statement_ExecuteWithDiagnostic)(&statement, &diagnostic);
  if (status != STATUS_CONST(SUCCESS)) goto cleanup;
  status = DATABASE_FUNC(Statement_Close)(&statement);
  if (status != STATUS_CONST(SUCCESS)) goto cleanup;
  status = DATABASE_FUNC(Prepare)(&database,
      "SELECT name, age FROM people", &statement);
  if (status != STATUS_CONST(SUCCESS)) goto cleanup;
  for (;;) {
    const char *name;
    size_t length;
    int64_t age;
    status = DATABASE_FUNC(Statement_NextWithDiagnostic)(&statement, &hasRow, &diagnostic);
    if (status != STATUS_CONST(SUCCESS) || !hasRow) break;
    status = DATABASE_FUNC(Statement_ColumnText)(&statement, 0, &name, &length);
    if (status != STATUS_CONST(SUCCESS)) break;
    status = DATABASE_FUNC(Statement_ColumnInt64)(&statement, 1, &age);
    if (status != STATUS_CONST(SUCCESS)) break;
    printf("%.*s: %lld\n", (int)length, name, (long long)age);
  }
cleanup:
  if (status != STATUS_CONST(SUCCESS)) {
    fprintf(stderr, "Database status %d: %s\n", (int)status, diagnostic.message);
  }
  {
    OPSTATUS closeStatement = DATABASE_FUNC(Statement_Close)(&statement);
    OPSTATUS closeDatabase = DATABASE_FUNC(Close)(&database);
    if (status == STATUS_CONST(SUCCESS)) {
      status = closeStatement != STATUS_CONST(SUCCESS) ? closeStatement : closeDatabase;
    }
  }
  return status == STATUS_CONST(SUCCESS) ? 0 : 1;
}
