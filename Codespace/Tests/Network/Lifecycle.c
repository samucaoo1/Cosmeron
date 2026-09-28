#include <assert.h>

#include "../../Cosmeron/Modules/Network/Connection/Connection.h"
#include "../../Cosmeron/Modules/Network/Listener/Listener.h"

int main(void) {
  TNetwork_Listener listener;
  TNetwork_Connection connection;
  TNetwork_Address loopback;
  TNetwork_Endpoint endpoint;
  uint16_t closed_port;
  TNetwork_Error error = NETWORK_ERROR_CONST(NONE);

  NETWORK_LISTENER_FUNC(Init)(&listener);
  NETWORK_CONNECTION_FUNC(Init)(&connection);
  NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&loopback);
  NETWORK_ADDRESS_FUNC(Endpoint_Create)(&endpoint, &loopback, 0);

  assert(NETWORK_LISTENER_FUNC(ListenAt)(&listener, &endpoint, NULL) ==
         STATUS_NS(SUCCESS));
  closed_port = NETWORK_LISTENER_FUNC(LocalEndpoint)(&listener)->port;
  NETWORK_LISTENER_FUNC(Close)(&listener);

  assert(NETWORK_CONNECTION_FUNC(Connect)(
             &connection, "127.0.0.1", closed_port, &error) != STATUS_NS(SUCCESS));
  assert(NETWORK_CONNECTION_FUNC(State)(&connection) ==
         NETWORK_CONNECTION_TYPE(FAILED));
  assert(error != NETWORK_ERROR_CONST(NONE));

  NETWORK_CONNECTION_FUNC(Close)(&connection);
  assert(NETWORK_CONNECTION_FUNC(State)(&connection) ==
         NETWORK_CONNECTION_TYPE(CLOSED));
  assert(NETWORK_CONNECTION_FUNC(LastError)(&connection) ==
         NETWORK_ERROR_CONST(NONE));
  NETWORK_CONNECTION_FUNC(Close)(&connection);

  NETWORK_CONNECTION_FUNC(Destroy)(&connection);
  return 0;
}
