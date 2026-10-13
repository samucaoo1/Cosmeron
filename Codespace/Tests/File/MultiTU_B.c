#include "../../Cosmeron/Modules/File/File.h"
int file_part_b(void) {
  FILE_TYPE(TFile) file = {0};
  return FILE_FUNC(Close)(&file) == STATUS_CONST(SUCCESS) ? 0 : 1;
}
