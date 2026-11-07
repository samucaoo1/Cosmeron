#define TYPE_ALIAS_SMALL
#include "../../Cosmeron/Modules/Type/Fundamental.h"

int main(void) {
  TYPE_NS(TI8) signedValue = 0;
  TYPE_NS(TU64) unsignedValue = 0U;
  TYPE_NS(TF32) realValue = 0.0f;
  TYPE_NS(TCh32) character = 0U;

  return signedValue != 0 || unsignedValue != 0U || realValue != 0.0f ||
         character != 0U;
}
