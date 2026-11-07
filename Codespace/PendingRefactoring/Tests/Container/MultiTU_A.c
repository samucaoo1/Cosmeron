#include "../../Cosmeron/Modules/Container/Array/Vector.h"
int container_multi_tu_a(void) {
  TVector(int, vector)
  Container_Flat_Vector_int_Destroy(&vector);
  return 0;
}

