#include "../../Cosmeron/Modules/Type/Fundamental.h"
#include "../../Cosmeron/Modules/Type/TBigint.h"
#include "../../Cosmeron/Modules/Type/TBlock.h"
#include "../../Cosmeron/Modules/Type/TDecimal.h"

int main(void) {
  TYPE_NS(TInt8) signedValue = 0;
  TYPE_NS(TUInt64) unsignedValue = 0U;
  TYPE_NS(TFloat32) realValue = 0.0f;
  TYPE_NS(TChar32) character = 0U;

  return signedValue != 0 || unsignedValue != 0U || realValue != 0.0f ||
         character != 0U;
}
