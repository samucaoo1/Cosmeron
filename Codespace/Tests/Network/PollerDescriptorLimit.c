#include <assert.h>

#include "../../Cosmeron/Modules/Network/Poll/Poll.h"

int main(void) {
#if OS_POSIX
  TNetwork_Poller poller;
  TNetwork_Event_Vector events;
  TNetwork_Datagram datagram;
  TNetwork_Error error = NETWORK_ERROR_CONST(NONE);

  NETWORK_POLLER_FUNC(Init)(&poller);
  assert(FLAT_VECTOR_FUNC(Network_Event, Init)(&events) == STATUS_NS(SUCCESS));
  NETWORK_DATAGRAM_FUNC(Init)(&datagram);

  datagram.socket.handle = (SOCKET_TYPE(Handle))FD_SETSIZE;

  assert(NETWORK_POLLER_FUNC(AddDatagram)(
             &poller, &datagram, NETWORK_EVENT_CONST(READ), NULL, NULL, NULL) ==
         STATUS_NS(SUCCESS));
  assert(NETWORK_POLLER_FUNC(Wait)(&poller, &events, &error) ==
         STATUS_NS(OUT_OF_RANGE));
  assert(error == NETWORK_ERROR_CONST(RESOURCE_EXHAUSTED));

  datagram.socket.handle = TSOCKET_NATIVE_INVALID;
  FLAT_VECTOR_FUNC(Network_Event, Destroy)(&events);
  NETWORK_POLLER_FUNC(Destroy)(&poller);
#endif
  return 0;
}
