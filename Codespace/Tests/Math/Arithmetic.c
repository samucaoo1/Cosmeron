#include "../../Cosmeron/Modules/Math/Arithmetic/Basic.h"
#include "../../Cosmeron/Modules/Math/Arithmetic/Clamp.h"

int main(void) {
  int32_t s=0;
  uint32_t u=0;
  if (Math_Arithmetic_Add_I32(2,3,&s)!=STATUS_CONST(SUCCESS) || s!=5) return 1;
  if (Math_Arithmetic_Sub_I32(2,3,&s)!=STATUS_CONST(SUCCESS) || s!=-1) return 2;
  if (Math_Arithmetic_Mul_I32(-4,5,&s)!=STATUS_CONST(SUCCESS) || s!=-20) return 3;
  if (Math_Arithmetic_Div_I32(9,2,&s)!=STATUS_CONST(SUCCESS) || s!=4) return 4;
  if (Math_Arithmetic_Mod_I32(9,4,&s)!=STATUS_CONST(SUCCESS) || s!=1) return 5;
  if (Math_Arithmetic_Add_I32(INT32_MAX,1,&s)!=STATUS_CONST(ARITHMETIC_OVERFLOW)) return 6;
  if (Math_Arithmetic_Sub_U32(0U,1U,&u)!=STATUS_CONST(ARITHMETIC_OVERFLOW)) return 7;
  if (Math_Arithmetic_Div_I32(1,0,&s)!=STATUS_CONST(DIVISION_BY_ZERO)) return 8;
  if (Math_Arithmetic_ClampAdd_I32(7,8,0,10)!=10) return 9;
  if (Math_Arithmetic_ClampSub_I32(2,9,0,10)!=0) return 10;
  if (Math_Arithmetic_ClampMul_I32(4,4,0,10)!=10) return 11;
  if (Math_Arithmetic_SaturatingAdd_I8(INT8_MAX,1)!=INT8_MAX) return 12;
  if (Math_Arithmetic_SaturatingSub_U8(0U,1U)!=0U) return 13;
  if (Math_Arithmetic_SaturatingMul_U8(UINT8_MAX,2U)!=UINT8_MAX) return 14;
  if (Math_Arithmetic_ClampAdd_I32(INT32_MIN,1,INT32_MIN,INT32_MIN)!=INT32_MIN) return 15;
  if (Math_Arithmetic_ClampAdd_I32(0,INT32_MIN,INT32_MAX,INT32_MAX)!=INT32_MAX) return 16;
  if (Math_Arithmetic_ClampMul_I32(-1,-1,INT32_MIN,INT32_MIN)!=INT32_MIN) return 17;
  return 0;
}
