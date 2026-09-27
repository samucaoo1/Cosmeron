#include "../../Cosmeron/Core/Memory/Operations.h"

#include <string.h>

int main(void) {
  char source[] = "Cosmeron";
  char destination[sizeof(source)] = {0};
  char overlap[] = "abcdef";
  int typedSource[] = {1, 2, 3, 4};
  int typedDestination[4] = {0};

  if (MEMORY_FUNC(Copy)(destination, source, sizeof(source)) !=
      STATUS_CONST(SUCCESS))
    return 1;
  if (strcmp(destination, source) != 0)
    return 2;

  if (MEMORY_FUNC(Move)(overlap + 1, overlap, 5) != STATUS_CONST(SUCCESS))
    return 3;
  if (MEMORY_FUNC(Set)(destination, 'X', sizeof(destination)) !=
          STATUS_CONST(SUCCESS) ||
      destination[0] != 'X')
    return 20;
  if (MEMORY_FUNC(Zero)(destination, sizeof(destination)) !=
      STATUS_CONST(SUCCESS))
    return 4;
  if (destination[0] != 0)
    return 5;

  if (Memory_CopyTyped(int, typedDestination, typedSource, 4) !=
      STATUS_CONST(SUCCESS))
    return 6;
  if (memcmp(typedDestination, typedSource, sizeof(typedSource)) != 0)
    return 7;

  if (Memory_MoveTyped(int, typedDestination + 1, typedDestination, 3) !=
      STATUS_CONST(SUCCESS))
    return 8;
  if (typedDestination[1] != 1 || typedDestination[2] != 2 ||
      typedDestination[3] != 3)
    return 9;

  if (Memory_ZeroTyped(int, typedDestination, 4) != STATUS_CONST(SUCCESS))
    return 10;
  if (typedDestination[0] != 0 || typedDestination[1] != 0 ||
      typedDestination[2] != 0 || typedDestination[3] != 0)
    return 11;

  if (MEMORY_FUNC(CopyArray)(typedDestination, typedSource, SIZE_MAX,
                             sizeof(int)) !=
      STATUS_CONST(ARITHMETIC_OVERFLOW))
    return 12;

  if (MEMORY_FUNC(Copy)(NULL, NULL, 0) != STATUS_CONST(SUCCESS))
    return 13;
  if (MEMORY_FUNC(Copy)(NULL, source, sizeof(source)) !=
      STATUS_CONST(INVALID_ARGUMENT))
    return 14;
  if (MEMORY_FUNC(Set)(NULL, 0, 0) != STATUS_CONST(SUCCESS))
    return 15;

  {
    int direct[] = {1, 2, 3, 4};
    if (MEMORY_FUNC(MoveArray)(direct + 1, direct, 3U, sizeof(int)) !=
        STATUS_CONST(SUCCESS))
      return 16;
    if (direct[1] != 1 || direct[2] != 2 || direct[3] != 3)
      return 17;
    if (MEMORY_FUNC(ZeroArray)(direct, 4U, sizeof(int)) !=
        STATUS_CONST(SUCCESS))
      return 18;
    if (direct[0] != 0 || direct[1] != 0 || direct[2] != 0 || direct[3] != 0)
      return 19;
  }

  return 0;
}
