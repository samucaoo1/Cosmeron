/* Match File.space: Core status declarations precede Windows headers. */
#include "../../Cosmeron/Core/Error/Status.h"
#include <stddef.h>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#else
#define FILE_CREATE 2
#define _TRUNCATE ((size_t)-1)
#endif
/* _TRUNCATE can expand during namespace separator concatenation. */
enum { originalCreate = FILE_CREATE };
static const size_t originalTruncate = (size_t)_TRUNCATE;
#include "../../Cosmeron/Modules/File/File.h"
#include <assert.h>
int main(void) {
  assert(FILE_CONST(OPEN_CREATE) == (1u << 2));
  assert(FILE_CONST(OPEN_TRUNCATE) == (1u << 3));
  assert(FILE_CREATE == originalCreate && (size_t)_TRUNCATE == originalTruncate);
  return 0;
}
