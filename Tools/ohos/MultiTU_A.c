#include "Cosmeron/Core/Preprocessor/Detect/OperationSystem.h"
#if !OS_OHOS
#error "OHOS native SDK must define __OHOS__"
#endif
int CosmeronOHOS_Detect(void) { return OS_KNOWN; }
