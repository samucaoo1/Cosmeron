#include <assert.h>

#include "../../Cosmeron/Modules/Network/Poll/Poll.h"

int main(void) {
#if OS_POSIX
  NETWORK_POLLER_TYPE(TPoller) poller;
  NETWORK_POLLER_TYPE(TEventVector) events;
  NETWORK_DATAGRAM_TYPE(TDatagram) datagram;
  NETWORK_TYPE(TError) error = NETWORK_ERROR_CONST(NONE);
  NETWORK_POLLER_TYPE(Handle) handle = 0;
  NETWORK_POLLER_TYPE(TEvent) sentinel = {0};

  NETWORK_POLLER_FUNC(Init)(&poller);
  assert(FLAT_VECTOR_FUNC(Network_Event, Init)(&events) == STATUS_CONST(SUCCESS));
  sentinel.handle = UINT64_C(777);
  sentinel.flags = NETWORK_EVENT_CONST(ERROR);
  assert(FLAT_VECTOR_FUNC(Network_Event, PushBack)(&events, sentinel) ==
         STATUS_CONST(SUCCESS));
  NETWORK_DATAGRAM_FUNC(Init)(&datagram);

  datagram.socket.handle = (SOCKET_TYPE(Handle))FD_SETSIZE;

  assert(NETWORK_POLLER_FUNC(AddDatagram)(
             &poller, &datagram, NETWORK_EVENT_CONST(READ), NULL, &handle, NULL) ==
         STATUS_CONST(SUCCESS));
  assert(handle != 0);
  assert(NETWORK_POLLER_FUNC(Wait)(&poller, &events, &error) ==
         STATUS_CONST(OUT_OF_RANGE));
  assert(error == NETWORK_ERROR_CONST(RESOURCE_EXHAUSTED));
  assert(events.size == 1U);
  assert(events.data[0].handle == UINT64_C(777));
  assert(events.data[0].flags == NETWORK_EVENT_CONST(ERROR));

  datagram.socket.handle = TSOCKET_NATIVE_INVALID;
  FLAT_VECTOR_FUNC(Network_Event, Destroy)(&events);
  NETWORK_POLLER_FUNC(Destroy)(&poller);
#endif
  return 0;
}
