# Tuple

`Modules/Tuple` provides pure POD tuple types (`Pair`, `Triple`, `Quad`, `Penta`, `Dual`) generated via dual-mode `.def` templates.

<!-- cosmeron-c11-test -->
```c
#include "Codespace/Cosmeron/Modules/Tuple/Tuple.h"
#include <assert.h>

int main(void) {
  MODULE_TYPED(Tuple, Pair, Int32) pair = { .first = 1, .second = 2 };
  assert(pair.first == 1 && pair.second == 2);
  return 0;
}
```
