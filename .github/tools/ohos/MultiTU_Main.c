#include <stdint.h>

uintptr_t CosmeronOHOS_Value(void);
int CosmeronOHOS_Detect(void);
int main(void) {
  return (CosmeronOHOS_Detect() == 0 || CosmeronOHOS_Value() == 0) ? 1 : 0;
}
