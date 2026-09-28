#include <assert.h>

#include "../../Cosmeron/Modules/Network/Poll/Poll.h"

int main(void) {
  NETWORK_LISTENER_TYPE(TListener) listener;
  NETWORK_POLLER_TYPE(TPoller) poller;
  NETWORK_POLLER_TYPE(TEventVector) events;
  NETWORK_ADDRESS_TYPE(TAddress) loopback;
  NETWORK_ADDRESS_TYPE(TEndpoint) endpoint;
  NETWORK_CONNECTION_TYPE(TConnection) client;
  NETWORK_POLLER_TYPE(Handle) handle = 0;

  NETWORK_LISTENER_FUNC(Init)(&listener);
  NETWORK_CONNECTION_FUNC(Init)(&client);
  NETWORK_POLLER_FUNC(Init)(&poller);
  FLAT_VECTOR_FUNC(Network_Event, Init)(&events);

  NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&loopback);
  NETWORK_ADDRESS_FUNC(Endpoint_Create)(&endpoint, &loopback, 0);
  assert(NETWORK_LISTENER_FUNC(ListenAt)(&listener, &endpoint, NULL) == STATUS_CONST(SUCCESS));
  endpoint = *NETWORK_LISTENER_FUNC(LocalEndpoint)(&listener);

  assert(NETWORK_POLLER_FUNC(AddListener)(
             &poller, &listener, NETWORK_EVENT_CONST(ACCEPT), NULL, &handle, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(handle != 0);
  assert(NETWORK_CONNECTION_FUNC(Connect)(&client, "127.0.0.1", endpoint.port, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(NETWORK_POLLER_FUNC(Wait)(&poller, &events, NULL) == STATUS_CONST(SUCCESS));
  assert(events.size == 1);
  assert(events.data[0].handle == handle);
  assert(NETWORK_POLLER_FUNC(Event_Has)(&events.data[0], NETWORK_EVENT_CONST(ACCEPT)));

  FLAT_VECTOR_FUNC(Network_Event, Destroy)(&events);
  NETWORK_POLLER_FUNC(Destroy)(&poller);
  NETWORK_CONNECTION_FUNC(Destroy)(&client);
  NETWORK_LISTENER_FUNC(Close)(&listener);
  return 0;
}
