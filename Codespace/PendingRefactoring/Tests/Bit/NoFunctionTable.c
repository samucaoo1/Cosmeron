#define PACKAGE_DISABLE_FUNCTION_TABLE 1
#include "../../Cosmeron/Modules/Bit/Bit.h"

int main(void) {
  uint32_t val = 0;
  if (Bit32_Set(&val, 2) != STATUS_CONST(SUCCESS))
    return 1;
  if (!Bit32_Check(val, 2))
    return 2;
  return 0;
}

