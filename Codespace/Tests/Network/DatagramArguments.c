#include <assert.h>

#include "../../Cosmeron/Modules/Network/Datagram/Datagram.h"

int main(void) {
  TNetwork_Datagram datagram;
  TNetwork_Address address;
  TNetwork_Endpoint endpoint;
  size_t count = 0;

  NETWORK_DATAGRAM_FUNC(Init)(&datagram);
  NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&address);
  NETWORK_ADDRESS_FUNC(Endpoint_Create)(&endpoint, &address, 1);

  assert(NETWORK_DATAGRAM_FUNC(SendTo)(
             &datagram, &endpoint, NULL, 1, &count, NULL) ==
         STATUS_NS(INVALID_ARGUMENT));
  assert(NETWORK_DATAGRAM_FUNC(ReceiveFrom)(
             &datagram, NULL, 1, NULL, &count, NULL) ==
         STATUS_NS(INVALID_ARGUMENT));
  assert(NETWORK_DATAGRAM_FUNC(Send)(
             &datagram, NULL, 1, &count, NULL) ==
         STATUS_NS(INVALID_ARGUMENT));
  assert(NETWORK_DATAGRAM_FUNC(Receive)(
             &datagram, NULL, 1, &count, NULL) ==
         STATUS_NS(INVALID_ARGUMENT));

  NETWORK_DATAGRAM_FUNC(Close)(&datagram);
  return 0;
}
