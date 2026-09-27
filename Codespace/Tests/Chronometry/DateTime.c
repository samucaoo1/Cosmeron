#include "../../Cosmeron/Modules/Chronometry/DateTime/DateTime.h"
int main(void){
  Chronometry_Calendar_Policy g=Chronometry_Calendar_SystemPolicy(CHRONOMETRY_CALENDAR_GREGORIAN);
  Chronometry_DateTime a={{2024,1,1},{500,30,15,10}}, b;
  Chronometry_Duration oneHour;
  if(!Chronometry_DateTime_TimeIsValid(a.time) || !Chronometry_DateTime_IsValid(&g,a)) return 1;
  if(Chronometry_Duration_FromSeconds(3600,&oneHour)!=STATUS_CONST(SUCCESS)) return 2;
  if(Chronometry_DateTime_AddDuration(&g,a,oneHour,&b)!=STATUS_CONST(SUCCESS)) return 3;
  if(b.time.hour!=11 || b.time.minute!=15 || b.time.second!=30 || b.time.millisecond!=500) return 4;
  Chronometry_Duration diff;
  if(Chronometry_DateTime_Difference(&g,a,b,&diff)!=STATUS_CONST(SUCCESS) || diff.nanoseconds!=INT64_C(3600000000000)) return 5;
  if(Chronometry_DateTime_Compare(a,b)>=0) return 6;
  return 0;
}
