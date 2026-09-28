#include <assert.h>

#include "../../Cosmeron/Modules/Network/Poll/Poll.h"

int main(void) {
  NETWORK_POLLER_TYPE(TPoller) poller;
  NETWORK_POLLER_TYPE(TEventVector) events;
  NETWORK_DATAGRAM_TYPE(TDatagram) datagram;
  NETWORK_ADDRESS_TYPE(TAddress) loopback;
  NETWORK_ADDRESS_TYPE(TEndpoint) endpoint;
  CHRONOMETRY_TYPE(Duration) timeout;
  NETWORK_POLLER_TYPE(Handle) handle = 0;

  NETWORK_POLLER_FUNC(Init)(&poller);
  assert(FLAT_VECTOR_FUNC(Network_Event, Init)(&events) == STATUS_CONST(SUCCESS));
  NETWORK_DATAGRAM_FUNC(Init)(&datagram);
  NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&loopback);
  NETWORK_ADDRESS_FUNC(Endpoint_Create)(&endpoint, &loopback, 0);
  assert(NETWORK_DATAGRAM_FUNC(Bind)(&datagram, &endpoint, NULL) == STATUS_CONST(SUCCESS));
  assert(NETWORK_POLLER_FUNC(AddDatagram)(
             &poller, &datagram, NETWORK_EVENT_CONST(READ), NULL, &handle, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(handle != 0);

  timeout = DURATION_FUNC(FromNanoseconds)(0);
  assert(NETWORK_POLLER_FUNC(WaitFor)(&poller, &events, timeout, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(events.size == 0);

  timeout = DURATION_FUNC(FromNanoseconds)(-1);
  assert(NETWORK_POLLER_FUNC(WaitFor)(&poller, &events, timeout, NULL) ==
         STATUS_CONST(INVALID_ARGUMENT));

  NETWORK_DATAGRAM_FUNC(Close)(&datagram);
  FLAT_VECTOR_FUNC(Network_Event, Destroy)(&events);
  NETWORK_POLLER_FUNC(Destroy)(&poller);
  return 0;
}
