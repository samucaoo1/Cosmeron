#include <assert.h>
#include <string.h>

#include "../../Cosmeron/Modules/Network/Poll/Poll.h"
#include "../../Cosmeron/Modules/Network/Resolve/Resolve.h"

static void test_address(void) {
  TNetwork_Address first, second;

  NETWORK_ADDRESS_FUNC(IPv4_Create)(&first, 127, 0, 0, 1);
  NETWORK_ADDRESS_FUNC(IPv4_Any)(&second);
  assert(!NETWORK_ADDRESS_FUNC(Equal)(&first, &second));

  NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&second);
  assert(NETWORK_ADDRESS_FUNC(Equal)(&first, &second));

  NETWORK_ADDRESS_FUNC(IPv6_Any)(&first);
  NETWORK_ADDRESS_FUNC(IPv6_Loopback)(&second);
  assert(!NETWORK_ADDRESS_FUNC(Equal)(&first, &second));
}

static void test_socket_stream(void) {
  TNetwork_Listener listener;
  TNetwork_Address loopback;
  TNetwork_Endpoint endpoint, peer;
  TSocket raw_listener, client, accepted;
  const char message[] = "surface";
  char buffer[16] = {0};
  size_t sent = 0, received = 0;

  NETWORK_LISTENER_FUNC(Init)(&listener);
  NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&loopback);
  NETWORK_ADDRESS_FUNC(Endpoint_Create)(&endpoint, &loopback, 0);

  assert(NETWORK_LISTENER_FUNC(ListenAt)(&listener, &endpoint, NULL) ==
         STATUS_CONST(SUCCESS));
  endpoint = *NETWORK_LISTENER_FUNC(LocalEndpoint)(&listener);
  assert(endpoint.port != 0);
  assert(NETWORK_LISTENER_FUNC(LastError)(&listener) ==
         NETWORK_ERROR_CONST(NONE));

  SOCKET_FUNC(Init)(&raw_listener);
  assert(SOCKET_FUNC(Create)(&raw_listener, SOCKET_TYPE(FAMILY_IPV4),
                             SOCKET_TYPE(KIND_STREAM),
                             SOCKET_TYPE(PROTOCOL_TCP), NULL) ==
         STATUS_CONST(SUCCESS));
  assert(SOCKET_FUNC(SetReuseAddress)(&raw_listener, true, NULL) ==
         STATUS_CONST(SUCCESS));
  {
    TNetwork_Endpoint any;
    NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&loopback);
    NETWORK_ADDRESS_FUNC(Endpoint_Create)(&any, &loopback, 0);
    assert(SOCKET_FUNC(Bind)(&raw_listener, &any, NULL) ==
           STATUS_CONST(SUCCESS));
    assert(SOCKET_FUNC(Listen)(&raw_listener, 1, NULL) ==
           STATUS_CONST(SUCCESS));
  }
  SOCKET_FUNC(Close)(&raw_listener);

  SOCKET_FUNC(Init)(&client);
  SOCKET_FUNC(Init)(&accepted);
  assert(SOCKET_FUNC(Create)(&client, SOCKET_TYPE(FAMILY_IPV4),
                             SOCKET_TYPE(KIND_STREAM),
                             SOCKET_TYPE(PROTOCOL_TCP), NULL) ==
         STATUS_CONST(SUCCESS));
  assert(SOCKET_FUNC(SetKeepAlive)(&client, true, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(SOCKET_FUNC(SetNoDelay)(&client, true, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(SOCKET_FUNC(SetBlocking)(&client, true, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(SOCKET_FUNC(Connect)(&client, &endpoint, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(SOCKET_FUNC(Accept)(&listener.primary, &accepted, &peer, NULL) ==
         STATUS_CONST(SUCCESS));

  received = 123U;
  assert(SOCKET_FUNC(Receive)(&accepted, NULL, 0U, &received, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(received == 0U);

  assert(SOCKET_FUNC(Send)(&client, message, sizeof(message), &sent, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(sent == sizeof(message));
  assert(SOCKET_FUNC(Receive)(&accepted, buffer, sizeof(buffer), &received,
                              NULL) == STATUS_CONST(SUCCESS));
  assert(received == sizeof(message));
  assert(memcmp(buffer, message, sizeof(message)) == 0);
  assert(SOCKET_FUNC(Shutdown)(&client, SOCKET_TYPE(SHUTDOWN_WRITE), NULL) ==
         STATUS_CONST(SUCCESS));

  SOCKET_FUNC(Close)(&accepted);
  SOCKET_FUNC(Close)(&client);
  NETWORK_LISTENER_FUNC(Close)(&listener);
}

static void test_socket_datagram(void) {
  TNetwork_Datagram receiver;
  TNetwork_Address loopback;
  TNetwork_Endpoint any, destination, sender_endpoint;
  TSocket sender;
  const char message[] = "datagram";
  char buffer[16] = {0};
  size_t sent = 0, received = 0;

  NETWORK_DATAGRAM_FUNC(Init)(&receiver);
  NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&loopback);
  NETWORK_ADDRESS_FUNC(Endpoint_Create)(&any, &loopback, 0);
  assert(NETWORK_DATAGRAM_FUNC(Bind)(&receiver, &any, NULL) ==
         STATUS_CONST(SUCCESS));
  destination = *NETWORK_DATAGRAM_FUNC(LocalEndpoint)(&receiver);

  SOCKET_FUNC(Init)(&sender);
  assert(SOCKET_FUNC(Create)(&sender, SOCKET_TYPE(FAMILY_IPV4),
                             SOCKET_TYPE(KIND_DATAGRAM),
                             SOCKET_TYPE(PROTOCOL_UDP), NULL) ==
         STATUS_CONST(SUCCESS));
  assert(SOCKET_FUNC(SendTo)(&sender, message, sizeof(message), &destination,
                             &sent, NULL) == STATUS_CONST(SUCCESS));
  assert(sent == sizeof(message));
  assert(SOCKET_FUNC(ReceiveFrom)(&receiver.socket, buffer, sizeof(buffer),
                                  &sender_endpoint, &received, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(received == sizeof(message));
  assert(memcmp(buffer, message, sizeof(message)) == 0);

  SOCKET_FUNC(Close)(&sender);
  NETWORK_DATAGRAM_FUNC(Close)(&receiver);
}

static void test_connection(void) {
  TNetwork_Listener listener;
  TNetwork_Connection client, server;
  TNetwork_Address loopback;
  TNetwork_Endpoint endpoint;
  const char byte = 'x';
  char received_byte = 0;
  size_t count = 0;

  NETWORK_LISTENER_FUNC(Init)(&listener);
  NETWORK_CONNECTION_FUNC(Init)(&client);
  NETWORK_CONNECTION_FUNC(Init)(&server);
  NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&loopback);
  NETWORK_ADDRESS_FUNC(Endpoint_Create)(&endpoint, &loopback, 0);

  assert(NETWORK_LISTENER_FUNC(ListenAt)(&listener, &endpoint, NULL) ==
         STATUS_CONST(SUCCESS));
  endpoint = *NETWORK_LISTENER_FUNC(LocalEndpoint)(&listener);

  assert(NETWORK_CONNECTION_FUNC(Connect)(&client, "127.0.0.1",
                                          endpoint.port, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(NETWORK_LISTENER_FUNC(Accept)(&listener, &server, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(NETWORK_CONNECTION_FUNC(State)(&client) ==
         NETWORK_CONNECTION_TYPE(CONNECTED));
  assert(NETWORK_CONNECTION_FUNC(LastError)(&client) ==
         NETWORK_ERROR_CONST(NONE));
  assert(NETWORK_CONNECTION_FUNC(LocalEndpoint)(&client)->port != 0);
  assert(NETWORK_CONNECTION_FUNC(RemoteEndpoint)(&client)->port == endpoint.port);

  assert(NETWORK_CONNECTION_FUNC(Write)(&client, &byte, 1U, &count, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(count == 1U);
  assert(NETWORK_CONNECTION_FUNC(Read)(&server, &received_byte, 1U, &count,
                                       NULL) == STATUS_CONST(SUCCESS));
  assert(count == 1U && received_byte == byte);

  NETWORK_CONNECTION_FUNC(Destroy)(&server);
  NETWORK_CONNECTION_FUNC(Close)(&client);
  assert(NETWORK_CONNECTION_FUNC(Reconnect)(&client, NULL) ==
         STATUS_CONST(SUCCESS));
  NETWORK_CONNECTION_FUNC(Init)(&server);
  assert(NETWORK_LISTENER_FUNC(Accept)(&listener, &server, NULL) ==
         STATUS_CONST(SUCCESS));

  NETWORK_CONNECTION_FUNC(Destroy)(&server);
  NETWORK_CONNECTION_FUNC(Destroy)(&client);
  NETWORK_LISTENER_FUNC(Close)(&listener);
}

static void test_datagram(void) {
  TNetwork_Datagram receiver, peer;
  TNetwork_Address loopback;
  TNetwork_Endpoint any, destination;
  const char message[] = "peer";
  char buffer[16] = {0};
  size_t sent = 0, received = 0;

  NETWORK_DATAGRAM_FUNC(Init)(&receiver);
  NETWORK_DATAGRAM_FUNC(Init)(&peer);
  NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&loopback);
  NETWORK_ADDRESS_FUNC(Endpoint_Create)(&any, &loopback, 0);

  assert(NETWORK_DATAGRAM_FUNC(Open)(&peer, SOCKET_TYPE(FAMILY_IPV4), NULL) ==
         STATUS_CONST(SUCCESS));
  assert(NETWORK_DATAGRAM_FUNC(LastError)(&peer) == NETWORK_ERROR_CONST(NONE));
  assert(NETWORK_DATAGRAM_FUNC(PeerEndpoint)(&peer) == NULL);
  assert(NETWORK_DATAGRAM_FUNC(SetBlocking)(&peer, false, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(NETWORK_DATAGRAM_FUNC(SetBlocking)(&peer, true, NULL) ==
         STATUS_CONST(SUCCESS));

  assert(NETWORK_DATAGRAM_FUNC(Bind)(&receiver, &any, NULL) ==
         STATUS_CONST(SUCCESS));
  destination = *NETWORK_DATAGRAM_FUNC(LocalEndpoint)(&receiver);
  assert(NETWORK_DATAGRAM_FUNC(SetPeer)(&peer, &destination, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(NETWORK_DATAGRAM_FUNC(PeerEndpoint)(&peer) != NULL);

  assert(NETWORK_DATAGRAM_FUNC(Send)(&peer, message, sizeof(message), &sent,
                                     NULL) == STATUS_CONST(SUCCESS));
  assert(sent == sizeof(message));
  assert(NETWORK_DATAGRAM_FUNC(ReceiveFrom)(
             &receiver, buffer, sizeof(buffer), NULL, &received, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(received == sizeof(message));

  assert(NETWORK_DATAGRAM_FUNC(ClearPeer)(&peer, NULL) ==
         STATUS_CONST(SUCCESS));
  NETWORK_DATAGRAM_FUNC(Close)(&peer);
  NETWORK_DATAGRAM_FUNC(Close)(&receiver);
}

static void test_poller_accessors(void) {
  TNetwork_Poller poller;
  TNetwork_Connection connection;
  TNetwork_Listener listener;
  TNetwork_Datagram datagram;
  TNetwork_Poll_Handle connection_handle = 0;
  TNetwork_Poll_Handle listener_handle = 0;
  TNetwork_Poll_Handle datagram_handle = 0;
  int marker = 42;
  TNetwork_Event event;

  NETWORK_POLLER_FUNC(Init)(&poller);
  NETWORK_CONNECTION_FUNC(Init)(&connection);
  NETWORK_LISTENER_FUNC(Init)(&listener);
  NETWORK_DATAGRAM_FUNC(Init)(&datagram);

  assert(NETWORK_POLLER_FUNC(AddConnection)(
             &poller, &connection, NETWORK_EVENT_CONST(READ), &marker,
             &connection_handle, NULL) == STATUS_CONST(SUCCESS));
  assert(NETWORK_POLLER_FUNC(AddListener)(
             &poller, &listener, NETWORK_EVENT_CONST(ACCEPT), NULL,
             &listener_handle, NULL) == STATUS_CONST(SUCCESS));
  assert(NETWORK_POLLER_FUNC(AddDatagram)(
             &poller, &datagram, NETWORK_EVENT_CONST(READ), NULL,
             &datagram_handle, NULL) == STATUS_CONST(SUCCESS));

  assert(NETWORK_POLLER_FUNC(Watch)(
             &poller, connection_handle, NETWORK_EVENT_CONST(WRITE), NULL) ==
         STATUS_CONST(SUCCESS));

  memset(&event, 0, sizeof(event));
  event.sourceType = NETWORK_POLLER_SOURCE_CONST(CONNECTION);
  event.flags = NETWORK_EVENT_CONST(WRITE);
  event.object = &connection;
  event.userdata = &marker;
  assert(NETWORK_POLLER_FUNC(Event_UserData)(&event) == &marker);
  assert(NETWORK_POLLER_FUNC(Event_Connection)(&event) == &connection);
  assert(NETWORK_POLLER_FUNC(Event_Listener)(&event) == NULL);
  assert(NETWORK_POLLER_FUNC(Event_Datagram)(&event) == NULL);

  event.sourceType = NETWORK_POLLER_SOURCE_CONST(LISTENER);
  event.object = &listener;
  assert(NETWORK_POLLER_FUNC(Event_Listener)(&event) == &listener);

  event.sourceType = NETWORK_POLLER_SOURCE_CONST(DATAGRAM);
  event.object = &datagram;
  assert(NETWORK_POLLER_FUNC(Event_Datagram)(&event) == &datagram);

  assert(NETWORK_POLLER_FUNC(Remove)(&poller, connection_handle, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(NETWORK_POLLER_FUNC(Remove)(&poller, listener_handle, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(NETWORK_POLLER_FUNC(Remove)(&poller, datagram_handle, NULL) ==
         STATUS_CONST(SUCCESS));

  NETWORK_POLLER_FUNC(Destroy)(&poller);
  NETWORK_DATAGRAM_FUNC(Close)(&datagram);
  NETWORK_LISTENER_FUNC(Close)(&listener);
  NETWORK_CONNECTION_FUNC(Destroy)(&connection);
}

static void test_listener_listen(void) {
  TNetwork_Listener listener;
  NETWORK_LISTENER_FUNC(Init)(&listener);
  assert(NETWORK_LISTENER_FUNC(Listen)(&listener, 0, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(NETWORK_LISTENER_FUNC(LocalEndpoint)(&listener)->port != 0);
  NETWORK_LISTENER_FUNC(Close)(&listener);
}

int main(void) {
  test_address();
  test_listener_listen();
  test_socket_stream();
  test_socket_datagram();
  test_connection();
  test_datagram();
  test_poller_accessors();
  return 0;
}
