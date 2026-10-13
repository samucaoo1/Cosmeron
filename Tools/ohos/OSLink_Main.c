#include "Cosmeron/Core/Error/Status.h"
OPSTATUS CosmeronOHOS_Clock(void);
OPSTATUS CosmeronOHOS_Loopback(void);
int main(void) {
  if (CosmeronOHOS_Clock() != STATUS_CONST(SUCCESS)) return 1;
  if (CosmeronOHOS_Loopback() != STATUS_CONST(SUCCESS)) return 2;
  return 0;
}
