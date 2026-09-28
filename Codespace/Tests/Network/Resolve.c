#include <assert.h>

#include "../../Cosmeron/Modules/Network/Resolve/Resolve.h"

int main(void) {
  TNetwork_Address_Vector addresses;
  TNetwork_Error error = NETWORK_ERROR_CONST(NONE);

  assert(FLAT_VECTOR_FUNC(Network_Address, Init)(&addresses) ==
         STATUS_NS(SUCCESS));
  assert(NETWORK_RESOLVE_FUNC(Address)(
             &addresses, "localhost", &error) == STATUS_NS(SUCCESS));
  assert(error == NETWORK_ERROR_CONST(NONE));
  assert(addresses.size > 0);

  FLAT_VECTOR_FUNC(Network_Address, Destroy)(&addresses);
  return 0;
}
