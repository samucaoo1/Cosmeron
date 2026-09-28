#include "../../Cosmeron/Modules/Network/Address/Address.h"

int network_multitu_a(void) {
  TNetwork_Address address;
  NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&address);
  return address.bytes.ipv4[0] == 127 ? 0 : 1;
}
