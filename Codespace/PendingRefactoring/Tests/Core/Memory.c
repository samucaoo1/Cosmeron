#include "../../Cosmeron/Core/Memory/Alloc.h"
#include "../../Cosmeron/Core/Memory/Swap.h"

#include <stdint.h>

int main(void) {
  int *values = NULL;
  int *original;
  int sentinelValue = 7;
  void *bytes = NULL;
  void *sentinel = &sentinelValue;
  OPSTATUS status;

  status = ALLOC_FUNC(Bytes)(&bytes, 16);
  if (status != STATUS_CONST(SUCCESS) || !bytes)
    return 1;

  status = ALLOC_FUNC(ReallocBytes)(&bytes, 32);
  if (status != STATUS_CONST(SUCCESS) || !bytes)
    return 16;

  status = ALLOC_FUNC(Free)(&bytes);
  if (status != STATUS_CONST(SUCCESS) || bytes != NULL)
    return 2;

  status = ALLOC_FUNC(Free)(NULL);
  if (status != STATUS_CONST(INVALID_ARGUMENT))
    return 3;

  bytes = sentinel;
  status = ALLOC_FUNC(Array)(&bytes, SIZE_MAX, sizeof(int));
  if (status != STATUS_CONST(ARITHMETIC_OVERFLOW) || bytes != sentinel)
    return 4;

  status = ALLOC_FUNC(Array)(&values, 4, sizeof(int));
  if (status != STATUS_CONST(SUCCESS) || !values)
    return 5;

  values[0] = 7;

  status = ALLOC_FUNC(ReallocArray)(&values, 8U, sizeof(int));
  if (status != STATUS_CONST(SUCCESS) || values == NULL)
    return 17;
  if (values[0] != 7)
    return 18;
  original = values;

  status = ALLOC_FUNC(ReallocArray)(&values, SIZE_MAX, sizeof(int));
  if (status != STATUS_CONST(ARITHMETIC_OVERFLOW) || values != original ||
      values[0] != 7)
    return 7;

  status = ALLOC_FUNC(Free)(&values);
  if (status != STATUS_CONST(SUCCESS) || values != NULL)
    return 8;

  {
    int left[] = {1, 2};
    int right[] = {3, 4};
    size_t i = 0;
    size_t j = 0;
    if (SWAP_FUNC(Bytes)(&left[i++], &right[j++], sizeof(int)) != STATUS_CONST(SUCCESS))
      return 9;
    if (i != 1 || j != 1 || left[0] != 3 || right[0] != 1)
      return 10;
  }

  return 0;
}
