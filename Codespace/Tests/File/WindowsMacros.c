/* Windows SDK collisions can also be checked by a POSIX preprocessor. */
#ifdef _WIN32
#include <windows.h>
#else
#define FILE_CREATE 2
#define FILE_TRUNCATE (0x2)
#endif
enum { originalCreate = FILE_CREATE, originalTruncate = FILE_TRUNCATE };
#include "../../Cosmeron/Modules/File/File.h"
#include <assert.h>
int main(void) {
  assert(FILE_CONST(OPEN_CREATE) == (1u << 2));
  assert(FILE_CONST(OPEN_TRUNCATE) == (1u << 3));
  assert(FILE_CREATE == originalCreate && FILE_TRUNCATE == originalTruncate);
  return 0;
}
