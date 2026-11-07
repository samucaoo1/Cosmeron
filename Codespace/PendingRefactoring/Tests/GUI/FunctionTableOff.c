#define GUI_DISABLE_FUNCTION_TABLE
#include "Test.h"
#ifdef GUI_FUNCTION_TABLE_STRUCT
#error Function table was emitted while disabled
#endif
int main(void) {
  GUI_TYPE(TContext) c = {0};
  return c.frame ? 1 : 0;
}
