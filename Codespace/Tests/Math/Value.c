#include "../../Cosmeron/Modules/Math/Value/Between.h"
#include "../../Cosmeron/Modules/Math/Value/Clamp.h"
#include "../../Cosmeron/Modules/Math/Value/MaxAndMin.h"

int main(void) {
  int32_t r=0;
  if (!Math_Value_IsBetween_I32(5,1,9)) return 1;
  if (Math_Value_IsBetween_I32(0,1,9)) return 2;
  if (Math_Value_Between_I32(1,1,9)!=MATH_RANGE_CONST(EQUAL_MINIMUM)) return 3;
  if (Math_Value_Between_I32(9,1,9)!=MATH_RANGE_CONST(EQUAL_MAXIMUM)) return 4;
  if (Math_Value_Between_I32(0,1,9)!=MATH_RANGE_CONST(LOWER)) return 5;
  if (Math_Value_Between_I32(10,1,9)!=MATH_RANGE_CONST(HIGHER)) return 6;
  if (Math_Value_Clamp_I32(-1,0,10)!=0 || Math_Value_Clamp_I32(11,0,10)!=10) return 7;
  if (Math_Value_Max_I32(2,3)!=3 || Math_Value_Min_I32(2,3)!=2) return 8;
  if (Math_Value_Smallest_I32(&r,4,7,3,9,5)!=STATUS_CONST(SUCCESS) || r!=3) return 9;
  if (Math_Value_Biggest_I32(&r,4,7,3,9,5)!=STATUS_CONST(SUCCESS) || r!=9) return 10;
  return 0;
}
