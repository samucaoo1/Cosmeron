#pragma once

#include <stdio.h>
#include <stdlib.h>

#define TEST_ASSERT(CONDITION)                                                \
  do {                                                                        \
    if (!(CONDITION)) {                                                       \
      fprintf(stderr, "%s:%d: assertion failed: %s\n", __FILE__, __LINE__,   \
              #CONDITION);                                                    \
      return 1;                                                               \
    }                                                                         \
  } while (0)

