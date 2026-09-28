#define COSMERON_NAMESPACE Cosmeron
#define COSMERON_NAMESPACE_CONST COSMERON
#include "../../Cosmeron/Modules/Text/Text.h"

#include <assert.h>

int main(void) {
  Cosmeron_Text_TGrid_char32 grid = {0};
  Cosmeron_Struct_TDual_uint16 size = {.col = 2U, .row = 1U};
  assert(Cosmeron_Text_Grid_char32_Create(&grid, size) ==
         COSMERON_STATUS_SUCCESS);
  assert(Cosmeron_Text_Grid_char32_Destroy(&grid) ==
         COSMERON_STATUS_SUCCESS);
  return 0;
}
