#include <assert.h>
#include <string.h>

#include "../../Cosmeron/Modules/Network/Listener/Listener.h"

int main(void) {
  NETWORK_LISTENER_TYPE(TListener) listener;
  NETWORK_CONNECTION_TYPE(TConnection) client, server;
  NETWORK_ADDRESS_TYPE(TAddress) loopback;
  NETWORK_ADDRESS_TYPE(TEndpoint) endpoint;
  char buffer[16] = {0};
  const char message[] = "cosmeron";

  NETWORK_LISTENER_FUNC(Init)(&listener);
  NETWORK_CONNECTION_FUNC(Init)(&client);
  NETWORK_CONNECTION_FUNC(Init)(&server);
  NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&loopback);
  NETWORK_ADDRESS_FUNC(Endpoint_Create)(&endpoint, &loopback, 0);

  assert(NETWORK_LISTENER_FUNC(ListenAt)(&listener, &endpoint, NULL) == STATUS_CONST(SUCCESS));
  endpoint = *NETWORK_LISTENER_FUNC(LocalEndpoint)(&listener);
  assert(endpoint.port != 0);

  {
    CHRONOMETRY_TYPE(Duration) timeout =
        DURATION_FUNC(FromNanoseconds)(2 * CHRONOMETRY_NANOSECONDS_PER_SECOND);
    assert(NETWORK_CONNECTION_FUNC(ConnectFor)(
               &client, "127.0.0.1", endpoint.port, timeout, NULL) ==
           STATUS_CONST(SUCCESS));
  }
  assert(NETWORK_LISTENER_FUNC(Accept)(&listener, &server, NULL) == STATUS_CONST(SUCCESS));

  assert(NETWORK_CONNECTION_FUNC(WriteAll)(&client, message, sizeof(message), NULL) ==
         STATUS_CONST(SUCCESS));
  assert(NETWORK_CONNECTION_FUNC(ReadExact)(&server, buffer, sizeof(message), NULL) ==
         STATUS_CONST(SUCCESS));
  assert(memcmp(buffer, message, sizeof(message)) == 0);
  assert(NETWORK_CONNECTION_FUNC(Shutdown)(
             &client, SOCKET_CONST(SHUTDOWN_WRITE), NULL) ==
         STATUS_CONST(SUCCESS));

  NETWORK_CONNECTION_FUNC(Destroy)(&server);
  NETWORK_CONNECTION_FUNC(Destroy)(&client);
  NETWORK_LISTENER_FUNC(Close)(&listener);
  return 0;
}
