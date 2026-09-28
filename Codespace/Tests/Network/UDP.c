#include <assert.h>
#include <string.h>

#include "../../Cosmeron/Modules/Network/Datagram/Datagram.h"

int main(void) {
  TNetwork_Datagram receiver, sender;
  TNetwork_Address loopback;
  TNetwork_Endpoint bind_endpoint, destination, source;
  const char message[] = "operation-skuld";
  char buffer[32] = {0};
  size_t sent = 0, received = 0;

  NETWORK_DATAGRAM_FUNC(Init)(&receiver);
  NETWORK_DATAGRAM_FUNC(Init)(&sender);
  NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&loopback);
  NETWORK_ADDRESS_FUNC(Endpoint_Create)(&bind_endpoint, &loopback, 0);

  assert(NETWORK_DATAGRAM_FUNC(Bind)(&receiver, &bind_endpoint, NULL) == STATUS_NS(SUCCESS));
  destination = *NETWORK_DATAGRAM_FUNC(LocalEndpoint)(&receiver);
  assert(destination.port != 0);

  assert(NETWORK_DATAGRAM_FUNC(SendTo)(&sender, &destination, message,
                                        sizeof(message), &sent, NULL) == STATUS_NS(SUCCESS));
  assert(sent == sizeof(message));
  assert(NETWORK_DATAGRAM_FUNC(ReceiveFrom)(&receiver, buffer, sizeof(buffer),
                                             &source, &received, NULL) == STATUS_NS(SUCCESS));
  assert(received == sizeof(message));
  assert(memcmp(buffer, message, sizeof(message)) == 0);

  assert(NETWORK_DATAGRAM_FUNC(SetPeer)(&sender, &destination, NULL) ==
         STATUS_NS(SUCCESS));
  assert(NETWORK_DATAGRAM_FUNC(SetPeer)(&receiver, &source, NULL) ==
         STATUS_NS(SUCCESS));
  memset(buffer, 0, sizeof(buffer));
  sent = 0;
  received = 0;
  assert(NETWORK_DATAGRAM_FUNC(Send)(&sender, message, sizeof(message), &sent,
                                     NULL) == STATUS_NS(SUCCESS));
  assert(sent == sizeof(message));
  assert(NETWORK_DATAGRAM_FUNC(Receive)(&receiver, buffer, sizeof(buffer),
                                        &received, NULL) == STATUS_NS(SUCCESS));
  assert(received == sizeof(message));
  assert(memcmp(buffer, message, sizeof(message)) == 0);

  NETWORK_DATAGRAM_FUNC(Close)(&sender);
  NETWORK_DATAGRAM_FUNC(Close)(&receiver);
  return 0;
}
