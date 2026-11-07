#include "../../Cosmeron/Modules/Network/Datagram/Datagram.h"

int network_multitu_b(void) {
  NETWORK_DATAGRAM_TYPE(TDatagram) datagram;
  NETWORK_DATAGRAM_FUNC(Init)(&datagram);
  NETWORK_DATAGRAM_FUNC(Close)(&datagram);
  return 0;
}
