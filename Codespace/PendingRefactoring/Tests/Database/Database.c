#include "../../Cosmeron/Modules/Database/Database.h"
#include <assert.h>
#include <math.h>

#define OK(OPERATION) assert((OPERATION) == STATUS_CONST(SUCCESS))

int main(void) {
  DATABASE_TYPE(TDatabase) database = {0};
  DATABASE_TYPE(TStatement) statement = {0};
  DATABASE_TYPE(TDiagnostic) diagnostic = {0};
  DATABASE_TYPE(ColumnKind) kind;
  char text[] = {'a', '\0', 'b'};
  unsigned char bytes[] = {0, 1, 255};
  const char *readText, *name;
  const void *readBlob;
  size_t length;
  int64_t number;
  double real;
  bool row = true, isNull;
  uint64_t affected;
  OK(DATABASE_FUNC(Open)(&database, ":memory:"));
  assert(DATABASE_FUNC(Open)(&database, ":memory:") == STATUS_CONST(BUSY));
  OK(DATABASE_FUNC(Execute)(&database,
      "CREATE TABLE items (id INTEGER PRIMARY KEY, n INTEGER, r REAL, t TEXT, b BLOB, z)"));
  OK(DATABASE_FUNC(Prepare)(&database,
      "INSERT INTO items (n,r,t,b,z) VALUES (?,?,?,?,?)", &statement));
  assert(DATABASE_FUNC(Statement_ParameterCount)(&statement) == 5);
  assert(DATABASE_FUNC(Statement_BindNull)(&statement, 0) == STATUS_CONST(OUT_OF_RANGE));
  assert(DATABASE_FUNC(Statement_BindNull)(&statement, SIZE_MAX) == STATUS_CONST(OUT_OF_RANGE));
  assert(DATABASE_FUNC(Statement_BindText)(&statement, 3, NULL, 1) == STATUS_CONST(INVALID_ARGUMENT));
  assert(DATABASE_FUNC(Statement_BindBlob)(&statement, 4, NULL, 1) == STATUS_CONST(INVALID_ARGUMENT));
  assert(DATABASE_FUNC(Statement_BindText)(&statement, 3, "x", SIZE_MAX) == STATUS_CONST(OUT_OF_RANGE));
  assert(DATABASE_FUNC(Statement_BindBlob)(&statement, 4, bytes, SIZE_MAX) == STATUS_CONST(OUT_OF_RANGE));
  assert(DATABASE_FUNC(Statement_BindDouble)(&statement, 2, NAN) == STATUS_CONST(INVALID_ARGUMENT));
  OK(DATABASE_FUNC(Statement_BindInt64)(&statement, 1, INT64_MIN));
  OK(DATABASE_FUNC(Statement_BindDouble)(&statement, 2, 1.25));
  OK(DATABASE_FUNC(Statement_BindText)(&statement, 3, text, sizeof(text)));
  OK(DATABASE_FUNC(Statement_BindBlob)(&statement, 4, bytes, sizeof(bytes)));
  OK(DATABASE_FUNC(Statement_BindNull)(&statement, 5));
  text[0] = 'X'; bytes[0] = 42; /* Bind copies both caller buffers. */
  OK(DATABASE_FUNC(Statement_Execute)(&statement));
  OK(DATABASE_FUNC(Statement_Execute)(&statement)); /* DONE must not insert again. */
  assert(DATABASE_FUNC(Statement_BindNull)(&statement, 1) == STATUS_CONST(INVALID_SEQUENCE));
  assert(DATABASE_FUNC(Statement_ClearBindings)(&statement) == STATUS_CONST(INVALID_SEQUENCE));
  OK(DATABASE_FUNC(AffectedRows)(&database, &affected)); assert(affected == 1);
  OK(DATABASE_FUNC(LastInsertId)(&database, &number)); assert(number == 1);
  OK(DATABASE_FUNC(Statement_Reset)(&statement));
  OK(DATABASE_FUNC(Statement_ClearBindings)(&statement));
  OK(DATABASE_FUNC(Statement_BindInt64)(&statement, 1, INT64_MAX));
  OK(DATABASE_FUNC(Statement_BindText)(&statement, 3, NULL, 0));
  OK(DATABASE_FUNC(Statement_BindBlob)(&statement, 4, NULL, 0));
  OK(DATABASE_FUNC(Statement_Next)(&statement, &row)); assert(!row);
  OK(DATABASE_FUNC(Statement_Close)(&statement));

  OK(DATABASE_FUNC(Prepare)(&database, "SELECT n,r,t,b,z FROM items ORDER BY id; -- tail\n /* ok */ ;", &statement));
  assert(DATABASE_FUNC(Statement_ColumnCount)(&statement) == 5);
  OK(DATABASE_FUNC(Statement_ColumnName)(&statement, 0, &name)); assert(!strcmp(name, "n"));
  number = 123;
  assert(DATABASE_FUNC(Statement_ColumnInt64)(&statement, 0, &number) == STATUS_CONST(INVALID_SEQUENCE));
  assert(number == 123);
  assert(DATABASE_FUNC(Close)(&database) == STATUS_CONST(BUSY));
  assert(database.native);
  assert(DATABASE_FUNC(Statement_Execute)(&statement) == STATUS_CONST(INVALID_ARGUMENT));
  OK(DATABASE_FUNC(Statement_Next)(&statement, &row)); assert(row);
  OK(DATABASE_FUNC(Statement_ColumnKind)(&statement, 0, &kind)); assert(kind == DATABASE_CONST(COLUMN_INTEGER));
  OK(DATABASE_FUNC(Statement_ColumnInt64)(&statement, 0, &number)); assert(number == INT64_MIN);
  OK(DATABASE_FUNC(Statement_ColumnDouble)(&statement, 0, &real)); assert(real < 0);
  OK(DATABASE_FUNC(Statement_ColumnDouble)(&statement, 1, &real)); assert(real == 1.25);
  assert(DATABASE_FUNC(Statement_ColumnInt64)(&statement, 1, &number) == STATUS_CONST(INVALID_ARGUMENT));
  assert(number == INT64_MIN);
  OK(DATABASE_FUNC(Statement_ColumnText)(&statement, 2, &readText, &length));
  assert(length == 3 && readText[0] == 'a' && readText[1] == '\0' && readText[2] == 'b');
  OK(DATABASE_FUNC(Statement_ColumnBlob)(&statement, 3, &readBlob, &length));
  assert(length == 3 && ((const unsigned char *)readBlob)[0] == 0);
  OK(DATABASE_FUNC(Statement_ColumnIsNull)(&statement, 4, &isNull)); assert(isNull);
  assert(DATABASE_FUNC(Statement_ColumnInt64)(&statement, 4, &number) == STATUS_CONST(INVALID_ARGUMENT));
  assert(DATABASE_FUNC(Statement_ColumnInt64)(&statement, SIZE_MAX, &number) == STATUS_CONST(OUT_OF_RANGE));
  assert(DATABASE_FUNC(Statement_ColumnText)(&statement, 3, &readText, &length) == STATUS_CONST(INVALID_ARGUMENT));
  OK(DATABASE_FUNC(Statement_Next)(&statement, &row)); assert(row);
  OK(DATABASE_FUNC(Statement_ColumnInt64)(&statement, 0, &number)); assert(number == INT64_MAX);
  OK(DATABASE_FUNC(Statement_ColumnText)(&statement, 2, &readText, &length)); assert(length == 0 && readText);
  OK(DATABASE_FUNC(Statement_ColumnBlob)(&statement, 3, &readBlob, &length)); assert(length == 0);
  OK(DATABASE_FUNC(Statement_ColumnIsNull)(&statement, 1, &isNull)); assert(isNull);
  OK(DATABASE_FUNC(Statement_Next)(&statement, &row)); assert(!row);
  OK(DATABASE_FUNC(Statement_Next)(&statement, &row)); assert(!row);
  OK(DATABASE_FUNC(Statement_Close)(&statement));

  assert(DATABASE_FUNC(Execute)(&database, "INSERT INTO items(n) VALUES(99); DELETE FROM items") == STATUS_CONST(INVALID_ARGUMENT));
  assert(DATABASE_FUNC(Execute)(&database, "INSERT INTO items(n) VALUES(99) RETURNING id") == STATUS_CONST(INVALID_ARGUMENT));
  assert(DATABASE_FUNC(Execute)(&database, "SELECT 1") == STATUS_CONST(INVALID_ARGUMENT));
  assert(DATABASE_FUNC(Prepare)(&database, " ; -- empty", &statement) == STATUS_CONST(INVALID_ARGUMENT));
  assert(DATABASE_FUNC(PrepareWithDiagnostic)(&database, "SELEC broken", &statement, &diagnostic) != STATUS_CONST(SUCCESS));
  assert(diagnostic.nativeCode && *diagnostic.message && !statement.native);
  OK(DATABASE_FUNC(Prepare)(&database, "SELECT count(*) FROM items", &statement));
  assert(DATABASE_FUNC(Prepare)(&database, "SELECT 1", &statement) == STATUS_CONST(BUSY));
  OK(DATABASE_FUNC(Statement_Next)(&statement, &row)); assert(row);
  OK(DATABASE_FUNC(Statement_ColumnInt64)(&statement, 0, &number)); assert(number == 2);
  OK(DATABASE_FUNC(Statement_Close)(&statement));

  OK(DATABASE_FUNC(Execute)(&database, "CREATE TABLE unique_values(n INTEGER UNIQUE)"));
  OK(DATABASE_FUNC(Prepare)(&database, "INSERT INTO unique_values VALUES(?)", &statement));
  OK(DATABASE_FUNC(Statement_BindInt64)(&statement, 1, 1));
  OK(DATABASE_FUNC(Statement_ExecuteWithDiagnostic)(&statement, &diagnostic));
  assert(!diagnostic.nativeCode && !*diagnostic.message);
  OK(DATABASE_FUNC(Statement_Reset)(&statement));
  assert(DATABASE_FUNC(Statement_NextWithDiagnostic)(&statement, &row, &diagnostic) != STATUS_CONST(SUCCESS));
  assert(!row && diagnostic.nativeCode && *diagnostic.message);
  assert(DATABASE_FUNC(Statement_Next)(&statement, &row) == STATUS_CONST(INVALID_SEQUENCE));
  assert(DATABASE_FUNC(Statement_Reset)(&statement) != STATUS_CONST(SUCCESS));
  OK(DATABASE_FUNC(Statement_BindInt64)(&statement, 1, 2));
  OK(DATABASE_FUNC(Statement_Execute)(&statement));
  OK(DATABASE_FUNC(Statement_Reset)(&statement));
  assert(DATABASE_FUNC(Statement_Execute)(&statement) != STATUS_CONST(SUCCESS));
  assert(DATABASE_FUNC(Statement_Close)(&statement) != STATUS_CONST(SUCCESS));
  assert(!statement.native); /* Finalize reports prior error but still destroys. */
  OK(DATABASE_FUNC(Statement_Close)(&statement));
  OK(DATABASE_FUNC(Close)(&database));
  OK(DATABASE_FUNC(Close)(&database));
  assert(DATABASE_FUNC(Close)(NULL) == STATUS_CONST(INVALID_ARGUMENT));
  assert(DATABASE_FUNC(Prepare)(NULL, "SELECT 1", &statement) == STATUS_CONST(INVALID_ARGUMENT));
  assert(DATABASE_FUNC(Statement_Next)(&statement, &row) == STATUS_CONST(INVALID_ARGUMENT));
  assert(!row);
  assert(DATABASE_FUNC(Statement_ColumnCount)(NULL) == 0);
  assert(DATABASE_FUNC(Statement_ParameterCount)(NULL) == 0);
  return 0;
}
