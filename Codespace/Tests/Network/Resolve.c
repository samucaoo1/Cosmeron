#include <assert.h>

#include "../../Cosmeron/Modules/Network/Resolve/Resolve.h"

int main(void) {
  NETWORK_ADDRESS_TYPE(TAddress)_Vector addresses;
  NETWORK_TYPE(Error) error = NETWORK_ERROR_CONST(NONE);

  assert(FLAT_VECTOR_FUNC(Network_Address, Init)(&addresses) ==
         STATUS_CONST(SUCCESS));
  assert(NETWORK_RESOLVE_FUNC(Address)(
             &addresses, "localhost", &error) == STATUS_CONST(SUCCESS));
  assert(error == NETWORK_ERROR_CONST(NONE));
  assert(addresses.size > 0);

  FLAT_VECTOR_FUNC(Network_Address, Destroy)(&addresses);
  return 0;
}
