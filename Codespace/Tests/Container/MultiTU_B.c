#include "../../Cosmeron/Modules/Container/Array/Vector.h"
int container_multi_tu_b(void) {
  TVector(int, vector)
  Flat_Vector_int_Destroy(&vector);
  return 0;
}

