#include "../../Cosmeron/Modules/Type/Type.h"

int main(void) {
  TYPE_NS(TInt8) signedValue = 0;
  TYPE_NS(TUInt64) unsignedValue = 0U;
  TYPE_NS(TFloat32) realValue = 0.0f;
  TYPE_NS(TChar32) character = 0U;
  TYPE_NS(TTag) tag = {0};

  return signedValue != 0 || unsignedValue != 0U || realValue != 0.0f ||
         character != 0U || tag.value != 0U;
}
