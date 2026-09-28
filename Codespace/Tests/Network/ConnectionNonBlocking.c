#include <assert.h>

#include "../../Cosmeron/Modules/Network/Listener/Listener.h"

int main(void) {
  NETWORK_LISTENER_TYPE(TListener) listener;
  NETWORK_CONNECTION_TYPE(TConnection) client, server;
  NETWORK_ADDRESS_TYPE(TAddress) loopback;
  NETWORK_ADDRESS_TYPE(TEndpoint) endpoint;
  char byte;
  size_t received = 123;
  NETWORK_TYPE(Error) error = NETWORK_ERROR_CONST(NONE);

  NETWORK_LISTENER_FUNC(Init)(&listener);
  NETWORK_CONNECTION_FUNC(Init)(&client);
  NETWORK_CONNECTION_FUNC(Init)(&server);
  NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&loopback);
  NETWORK_ADDRESS_FUNC(Endpoint_Create)(&endpoint, &loopback, 0);

  assert(NETWORK_LISTENER_FUNC(ListenAt)(&listener, &endpoint, NULL) ==
         STATUS_CONST(SUCCESS));
  endpoint = *NETWORK_LISTENER_FUNC(LocalEndpoint)(&listener);

  assert(NETWORK_CONNECTION_FUNC(SetBlocking)(&client, false, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(NETWORK_CONNECTION_FUNC(Connect)(&client, "127.0.0.1", endpoint.port, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(!client.blocking);
  assert(!client.socket.blocking);

  assert(NETWORK_LISTENER_FUNC(Accept)(&listener, &server, NULL) ==
         STATUS_CONST(SUCCESS));

  assert(NETWORK_CONNECTION_FUNC(Read)(&client, &byte, sizeof(byte), &received, &error) !=
         STATUS_CONST(SUCCESS));
  assert(received == 123);
  assert(error == NETWORK_ERROR_CONST(WOULD_BLOCK));
  assert(NETWORK_CONNECTION_FUNC(State)(&client) ==
         NETWORK_CONNECTION_CONST(CONNECTED));

  NETWORK_CONNECTION_FUNC(Destroy)(&server);
  NETWORK_CONNECTION_FUNC(Destroy)(&client);
  NETWORK_LISTENER_FUNC(Close)(&listener);
  return 0;
}
