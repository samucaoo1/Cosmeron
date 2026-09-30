#include <assert.h>

#include "../../Cosmeron/Modules/Network/Resolve/Resolve.h"

int main(void) {
  NETWORK_RESOLVE_TYPE(TAddressVector) addresses;
  NETWORK_ADDRESS_TYPE(TAddress) sentinel;
  NETWORK_TYPE(TError) error = NETWORK_ERROR_CONST(NONE);

  assert(FLAT_VECTOR_FUNC(Network_Address, Init)(&addresses) ==
         STATUS_CONST(SUCCESS));
  CONTAINER_API_BIND(addresses, FLAT_VECTOR_FUNC(Network_Address, functions));

  NETWORK_ADDRESS_FUNC(IPv4_Create)(&sentinel, 203U, 0U, 113U, 1U);
  assert(FLAT_VECTOR_FUNC(Network_Address, PushBack)(&addresses, sentinel) ==
         STATUS_CONST(SUCCESS));

  error = NETWORK_ERROR_CONST(UNKNOWN);
  assert(NETWORK_RESOLVE_FUNC(Address)(&addresses, NULL, &error) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(addresses.size == 1U);
  assert(NETWORK_ADDRESS_FUNC(Equal)(&addresses.data[0], &sentinel));
#if COSMERON_MACRO_INTERNAL_CONTAINER_FUNCTION_TABLE_ENABLED
  assert(addresses.api == &FLAT_VECTOR_FUNC(Network_Address, functions));
#endif

  assert(NETWORK_RESOLVE_FUNC(Address)(
             &addresses, "localhost", &error) == STATUS_CONST(SUCCESS));
  assert(error == NETWORK_ERROR_CONST(NONE));
  assert(addresses.size > 0U);
#if COSMERON_MACRO_INTERNAL_CONTAINER_FUNCTION_TABLE_ENABLED
  assert(addresses.api == &FLAT_VECTOR_FUNC(Network_Address, functions));
#endif

  for (size_t index = 0; index < addresses.size; ++index)
    assert(!NETWORK_ADDRESS_FUNC(Equal)(&addresses.data[index], &sentinel));

  FLAT_VECTOR_FUNC(Network_Address, Destroy)(&addresses);
  return 0;
}
