#include "../../Cosmeron/Modules/Sort/Sort.h"

int main(void) {
  const struct Sort_FunctionTable *table = Sort_GetFunctionTable();
  return table != NULL ? 0 : 1;
}

