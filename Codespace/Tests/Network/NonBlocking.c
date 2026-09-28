#include <assert.h>

#include "../../Cosmeron/Modules/Network/Datagram/Datagram.h"

int main(void) {
  TNetwork_Datagram datagram;
  TNetwork_Address loopback;
  TNetwork_Endpoint endpoint;
  char byte;
  size_t received = 123;
  TNetwork_Error error = NETWORK_ERROR_CONST(NONE);

  NETWORK_DATAGRAM_FUNC(Init)(&datagram);
  NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&loopback);
  NETWORK_ADDRESS_FUNC(Endpoint_Create)(&endpoint, &loopback, 0);

  assert(NETWORK_DATAGRAM_FUNC(Bind)(&datagram, &endpoint, NULL) ==
         STATUS_NS(SUCCESS));
  assert(NETWORK_DATAGRAM_FUNC(SetBlocking)(&datagram, false, NULL) ==
         STATUS_NS(SUCCESS));
  assert(NETWORK_DATAGRAM_FUNC(ReceiveFrom)(
             &datagram, &byte, sizeof(byte), NULL, &received, &error) !=
         STATUS_NS(SUCCESS));
  assert(received == 123);
  assert(error == NETWORK_ERROR_CONST(WOULD_BLOCK));

  NETWORK_DATAGRAM_FUNC(Close)(&datagram);
  return 0;
}
