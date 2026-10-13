#include "../../Cosmeron/Modules/File/File.h"
int file_part_a(void) { return FILE_FUNC(Path_IsAbsolute)("relative") ? 1 : 0; }
