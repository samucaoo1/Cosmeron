#include <assert.h>

#include "../../Cosmeron/Modules/Network/Listener/Listener.h"

int main(void) {
  TNetwork_Listener listener;
  TNetwork_Connection client, server;
  TNetwork_Address loopback;
  TNetwork_Endpoint endpoint;
  char byte;
  size_t received = 123;
  TNetwork_Error error = NETWORK_ERROR_CONST(NONE);

  NETWORK_LISTENER_FUNC(Init)(&listener);
  NETWORK_CONNECTION_FUNC(Init)(&client);
  NETWORK_CONNECTION_FUNC(Init)(&server);
  NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&loopback);
  NETWORK_ADDRESS_FUNC(Endpoint_Create)(&endpoint, &loopback, 0);

  assert(NETWORK_LISTENER_FUNC(ListenAt)(&listener, &endpoint, NULL) ==
         STATUS_NS(SUCCESS));
  endpoint = *NETWORK_LISTENER_FUNC(LocalEndpoint)(&listener);

  assert(NETWORK_CONNECTION_FUNC(SetBlocking)(&client, false, NULL) ==
         STATUS_NS(SUCCESS));
  assert(NETWORK_CONNECTION_FUNC(Connect)(&client, "127.0.0.1", endpoint.port, NULL) ==
         STATUS_NS(SUCCESS));
  assert(!client.blocking);
  assert(!client.socket.blocking);

  assert(NETWORK_LISTENER_FUNC(Accept)(&listener, &server, NULL) ==
         STATUS_NS(SUCCESS));

  assert(NETWORK_CONNECTION_FUNC(Read)(&client, &byte, sizeof(byte), &received, &error) !=
         STATUS_NS(SUCCESS));
  assert(received == 123);
  assert(error == NETWORK_ERROR_CONST(WOULD_BLOCK));
  assert(NETWORK_CONNECTION_FUNC(State)(&client) ==
         NETWORK_CONNECTION_TYPE(CONNECTED));

  NETWORK_CONNECTION_FUNC(Destroy)(&server);
  NETWORK_CONNECTION_FUNC(Destroy)(&client);
  NETWORK_LISTENER_FUNC(Close)(&listener);
  return 0;
}
