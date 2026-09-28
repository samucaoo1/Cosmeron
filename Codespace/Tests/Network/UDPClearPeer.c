#include <assert.h>
#include <string.h>

#include "../../Cosmeron/Modules/Network/Datagram/Datagram.h"

int main(void) {
  NETWORK_DATAGRAM_TYPE(TDatagram) first, second, sender;
  NETWORK_ADDRESS_TYPE(TAddress) loopback;
  NETWORK_ADDRESS_TYPE(TEndpoint) any, first_endpoint, second_endpoint;
  const char message[] = "el-psy-kongroo";
  char buffer[32] = {0};
  size_t sent = 0, received = 0;

  NETWORK_DATAGRAM_FUNC(Init)(&first);
  NETWORK_DATAGRAM_FUNC(Init)(&second);
  NETWORK_DATAGRAM_FUNC(Init)(&sender);
  NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&loopback);
  NETWORK_ADDRESS_FUNC(Endpoint_Create)(&any, &loopback, 0);

  assert(NETWORK_DATAGRAM_FUNC(Bind)(&first, &any, NULL) == STATUS_CONST(SUCCESS));
  assert(NETWORK_DATAGRAM_FUNC(Bind)(&second, &any, NULL) == STATUS_CONST(SUCCESS));
  first_endpoint = *NETWORK_DATAGRAM_FUNC(LocalEndpoint)(&first);
  second_endpoint = *NETWORK_DATAGRAM_FUNC(LocalEndpoint)(&second);

  assert(NETWORK_DATAGRAM_FUNC(SetPeer)(&sender, &first_endpoint, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(NETWORK_DATAGRAM_FUNC(HasPeer)(&sender));
  assert(NETWORK_DATAGRAM_FUNC(ClearPeer)(&sender, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(!NETWORK_DATAGRAM_FUNC(HasPeer)(&sender));

  assert(NETWORK_DATAGRAM_FUNC(SendTo)(
             &sender, &second_endpoint, message, sizeof(message), &sent, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(sent == sizeof(message));
  assert(NETWORK_DATAGRAM_FUNC(ReceiveFrom)(
             &second, buffer, sizeof(buffer), NULL, &received, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(received == sizeof(message));
  assert(memcmp(buffer, message, sizeof(message)) == 0);

  NETWORK_DATAGRAM_FUNC(Close)(&sender);
  NETWORK_DATAGRAM_FUNC(Close)(&second);
  NETWORK_DATAGRAM_FUNC(Close)(&first);
  return 0;
}
