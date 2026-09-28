#include <assert.h>
#include <string.h>

#include "../../Cosmeron/Modules/Network/Listener/Listener.h"
#include "../../Cosmeron/Modules/Network/Datagram/Datagram.h"

int main(void) {
  TNetwork_Listener listener;
  TNetwork_Datagram datagram;
  TNetwork_Address address;
  TNetwork_Endpoint endpoint;

  NETWORK_LISTENER_FUNC(Init)(&listener);
  NETWORK_DATAGRAM_FUNC(Init)(&datagram);

  NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&address);
  NETWORK_ADDRESS_FUNC(Endpoint_Create)(&endpoint, &address, 0);

  assert(NETWORK_LISTENER_FUNC(ListenAt)(&listener, &endpoint, NULL) == STATUS_NS(SUCCESS));
  assert(NETWORK_LISTENER_FUNC(LocalEndpoint)(&listener)->port != 0);
  NETWORK_LISTENER_FUNC(Close)(&listener);

  assert(NETWORK_DATAGRAM_FUNC(Bind)(&datagram, &endpoint, NULL) == STATUS_NS(SUCCESS));
  assert(NETWORK_DATAGRAM_FUNC(LocalEndpoint)(&datagram)->port != 0);
  NETWORK_DATAGRAM_FUNC(Close)(&datagram);
  return 0;
}
