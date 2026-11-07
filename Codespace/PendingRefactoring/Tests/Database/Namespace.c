#define COSMERON_NAMESPACE Lab
#define COSMERON_NAMESPACE_CONST LAB
#include "../../Cosmeron/Modules/Database/Database.h"
#include <assert.h>
int main(void) {
  Lab_Database_TDatabase database = {0};
  Lab_Database_TStatement statement = {0};
  int64_t value = 0;
  bool row;
  assert(Lab_Database_Open(&database, ":memory:") == LAB_STATUS_SUCCESS);
  assert(Lab_Database_Prepare(&database, "SELECT 42", &statement) == LAB_STATUS_SUCCESS);
  assert(Lab_Database_Statement_Next(&statement, &row) == LAB_STATUS_SUCCESS && row);
  assert(Lab_Database_Statement_ColumnInt64(&statement, 0, &value) == LAB_STATUS_SUCCESS && value == 42);
  assert(Lab_Database_Statement_Close(&statement) == LAB_STATUS_SUCCESS);
  assert(Lab_Database_Close(&database) == LAB_STATUS_SUCCESS);
  return 0;
}
