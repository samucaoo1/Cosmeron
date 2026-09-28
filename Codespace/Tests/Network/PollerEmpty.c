#include <assert.h>

#include "../../Cosmeron/Modules/Network/Poll/Poll.h"

int main(void) {
  TNetwork_Poller poller;
  TNetwork_Event_Vector events;

  NETWORK_POLLER_FUNC(Init)(&poller);
  assert(FLAT_VECTOR_FUNC(Network_Event, Init)(&events) == STATUS_NS(SUCCESS));

  assert(NETWORK_POLLER_FUNC(Wait)(&poller, &events, NULL) == STATUS_NS(SUCCESS));
  assert(events.size == 0);

  FLAT_VECTOR_FUNC(Network_Event, Destroy)(&events);
  NETWORK_POLLER_FUNC(Destroy)(&poller);
  return 0;
}
