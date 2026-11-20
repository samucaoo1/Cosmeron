#include "../../Cosmeron/Core/Types/ByteTag.h"
#include "../../Cosmeron/Core/Types/Optional.h"
#include "../../Cosmeron/Core/Types/Result.h"

#include <stdio.h>

TEMPLATE_INSTANTIATE(Optional, Int32, int32_t);
TEMPLATE_INSTANTIATE(Result, Int32, int32_t);

int main(void) {
  /* 1. Optional<Int32> */
  MODULE_TYPED(Optional, Int32) maybe = {0};
  (void)MODULE_FUNC(Optional, Int32, Some)(&maybe, 42);
  if (OBJECT_CALL_0(&maybe, IsSome)) {
    int32_t value = 0;
    (void)OBJECT_CALL(&maybe, Value, &value);
    printf("Optional has value: %d\n", (int)value);
  }

  /* 2. Result<Int32> */
  MODULE_TYPED(Result, Int32) res = {0};
  (void)MODULE_FUNC(Result, Int32, Ok)(&res, 100);
  if (OBJECT_CALL_0(&res, IsOk)) {
    int32_t okVal = 0;
    (void)OBJECT_CALL(&res, Value, &okVal);
    printf("Result is Ok: %d\n", (int)okVal);
  }

  /* 3. ByteTag32 (FourCC) */
  MODULE_TYPE(Type, ByteTag32) tag = {0};
  (void)Type_ByteTag32_Init(&tag);
  (void)OBJECT_CALL(&tag, FromString, "RIFF");
  char text[8] = {0};
  (void)OBJECT_CALL(&tag, ToString, text, sizeof(text));
  printf("ByteTag32: %s (0x%08X)\n", text, OBJECT_CALL_0(&tag, ToUInt));

  return 0;
}
