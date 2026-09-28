#include <assert.h>

#include "../../Cosmeron/Modules/Network/Poll/Poll.h"

int main(void) {
  TNetwork_Listener listener;
  TNetwork_Connection client, server;
  TNetwork_Poller poller;
  TNetwork_Event_Vector events;
  TNetwork_Address loopback;
  TNetwork_Endpoint endpoint;
  TNetwork_Poll_Handle handle = 0;

  NETWORK_LISTENER_FUNC(Init)(&listener);
  NETWORK_CONNECTION_FUNC(Init)(&client);
  NETWORK_CONNECTION_FUNC(Init)(&server);
  NETWORK_POLLER_FUNC(Init)(&poller);
  assert(FLAT_VECTOR_FUNC(Network_Event, Init)(&events) == STATUS_NS(SUCCESS));

  NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&loopback);
  NETWORK_ADDRESS_FUNC(Endpoint_Create)(&endpoint, &loopback, 0);
  assert(NETWORK_LISTENER_FUNC(ListenAt)(&listener, &endpoint, NULL) ==
         STATUS_NS(SUCCESS));
  endpoint = *NETWORK_LISTENER_FUNC(LocalEndpoint)(&listener);

  assert(NETWORK_CONNECTION_FUNC(Connect)(&client, "127.0.0.1", endpoint.port, NULL) ==
         STATUS_NS(SUCCESS));
  assert(NETWORK_LISTENER_FUNC(Accept)(&listener, &server, NULL) ==
         STATUS_NS(SUCCESS));

  assert(NETWORK_POLLER_FUNC(AddConnection)(
             &poller, &server,
             NETWORK_EVENT_CONST(READ) | NETWORK_EVENT_CONST(CLOSE),
             NULL, &handle, NULL) == STATUS_NS(SUCCESS));

  NETWORK_CONNECTION_FUNC(Close)(&client);
  assert(NETWORK_POLLER_FUNC(Wait)(&poller, &events, NULL) == STATUS_NS(SUCCESS));
  assert(events.size == 1);
  assert(events.data[0].handle == handle);
  assert(NETWORK_POLLER_FUNC(Event_Has)(&events.data[0],
                                        NETWORK_EVENT_CONST(CLOSE)));
  assert(!NETWORK_POLLER_FUNC(Event_Has)(&events.data[0],
                                         NETWORK_EVENT_CONST(READ)));

  FLAT_VECTOR_FUNC(Network_Event, Destroy)(&events);
  NETWORK_POLLER_FUNC(Destroy)(&poller);
  NETWORK_CONNECTION_FUNC(Destroy)(&server);
  NETWORK_CONNECTION_FUNC(Destroy)(&client);
  NETWORK_LISTENER_FUNC(Close)(&listener);
  return 0;
}
