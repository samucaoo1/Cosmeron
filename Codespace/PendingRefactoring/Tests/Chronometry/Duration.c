#include "../../Cosmeron/Modules/Chronometry/Duration/Duration.h"
#include <limits.h>
int main(void){
  Chronometry_Duration a=Chronometry_Duration_FromNanoseconds(5), b, r;
  if(a.nanoseconds!=5) return 1;
  if(Chronometry_Duration_FromSeconds(2,&b)!=STATUS_CONST(SUCCESS) || b.nanoseconds!=INT64_C(2000000000)) return 2;
  if(Chronometry_Duration_Add(a,b,&r)!=STATUS_CONST(SUCCESS) || r.nanoseconds!=INT64_C(2000000005)) return 3;
  if(Chronometry_Duration_Subtract(b,a,&r)!=STATUS_CONST(SUCCESS) || r.nanoseconds!=INT64_C(1999999995)) return 4;
  if(Chronometry_Duration_Compare(a,b)>=0) return 5;
  a.nanoseconds=-7;
  if(Chronometry_Duration_Absolute(a,&r)!=STATUS_CONST(SUCCESS) || r.nanoseconds!=7) return 6;
  a.nanoseconds=INT64_MIN;
  if(Chronometry_Duration_Absolute(a,&r)!=STATUS_CONST(ARITHMETIC_OVERFLOW)) return 7;
  return 0;
}
