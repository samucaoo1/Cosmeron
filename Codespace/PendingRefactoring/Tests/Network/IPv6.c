#include <assert.h>
#include <string.h>

#include "../../Cosmeron/Modules/Network/Listener/Listener.h"

int main(void) {
  NETWORK_LISTENER_TYPE(TListener) listener;
  NETWORK_CONNECTION_TYPE(TConnection) client, server;
  NETWORK_ADDRESS_TYPE(TAddress) loopback;
  NETWORK_ADDRESS_TYPE(TEndpoint) endpoint;
  const char message[] = "divergence";
  char buffer[sizeof(message)] = {0};

  NETWORK_LISTENER_FUNC(Init)(&listener);
  NETWORK_CONNECTION_FUNC(Init)(&client);
  NETWORK_CONNECTION_FUNC(Init)(&server);
  NETWORK_ADDRESS_FUNC(IPv6_Loopback)(&loopback);
  NETWORK_ADDRESS_FUNC(Endpoint_Create)(&endpoint, &loopback, 0);

  if (NETWORK_LISTENER_FUNC(ListenAt)(&listener, &endpoint, NULL) !=
      STATUS_CONST(SUCCESS))
    return 0; /* IPv6 is optional on the host running the test. */

  assert(NETWORK_LISTENER_FUNC(LocalEndpoint)(&listener, &endpoint) ==
         STATUS_CONST(SUCCESS));
  assert(endpoint.address.type == NETWORK_ADDRESS_CONST(IPV6));
  assert(endpoint.port != 0);

  assert(NETWORK_CONNECTION_FUNC(Connect)(&client, "::1", endpoint.port, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(NETWORK_LISTENER_FUNC(Accept)(&listener, &server, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(NETWORK_CONNECTION_FUNC(WriteAll)(&client, message, sizeof(message), NULL) ==
         STATUS_CONST(SUCCESS));
  assert(NETWORK_CONNECTION_FUNC(ReadExact)(&server, buffer, sizeof(buffer), NULL) ==
         STATUS_CONST(SUCCESS));
  assert(memcmp(buffer, message, sizeof(message)) == 0);

  NETWORK_CONNECTION_FUNC(Destroy)(&server);
  NETWORK_CONNECTION_FUNC(Destroy)(&client);
  NETWORK_LISTENER_FUNC(Close)(&listener);
  return 0;
}
