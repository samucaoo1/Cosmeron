#include "../../Cosmeron/Modules/Chronometry/Instant/Instant.h"
int main(void){
  Chronometry_Instant a={10,900000000U}, b, c={12,100000000U};
  Chronometry_Duration d={300000000};
  if(Chronometry_Instant_AddDuration(a,d,&b)!=STATUS_CONST(SUCCESS)) return 1;
  if(b.seconds!=11 || b.nanoseconds!=200000000U) return 2;
  if(Chronometry_Instant_Compare(a,b)>=0) return 3;
  if(Chronometry_Instant_DurationBetween(a,c,&d)!=STATUS_CONST(SUCCESS)) return 4;
  if(d.nanoseconds!=1200000000LL) return 5;
  return 0;
}
