#include <assert.h>

#include "../../Cosmeron/Modules/Network/Poll/Poll.h"

int main(void) {
  NETWORK_POLLER_TYPE(TPoller) poller;
  NETWORK_POLLER_TYPE(TEventVector) events;

  NETWORK_POLLER_FUNC(Init)(&poller);
  assert(FLAT_VECTOR_FUNC(Network_Event, Init)(&events) == STATUS_CONST(SUCCESS));

  assert(NETWORK_POLLER_FUNC(Wait)(&poller, &events, NULL) == STATUS_CONST(SUCCESS));
  assert(events.size == 0);

  FLAT_VECTOR_FUNC(Network_Event, Destroy)(&events);
  NETWORK_POLLER_FUNC(Destroy)(&poller);
  return 0;
}
