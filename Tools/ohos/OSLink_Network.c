#include "Cosmeron/Modules/Network/Resolve/Resolve.h"
OPSTATUS CosmeronOHOS_Loopback(void) {
  NETWORK_RESOLVE_TYPE(TAddressVector) addresses = {0};
  OPSTATUS result = NETWORK_RESOLVE_FUNC(Address)(&addresses, "localhost", NULL);
  FLAT_VECTOR_FUNC(Network_Address, Destroy)(&addresses);
  return result;
}
