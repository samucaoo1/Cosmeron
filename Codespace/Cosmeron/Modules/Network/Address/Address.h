#pragma once

#include "../Network.space"
#include <string.h>

typedef enum NETWORK_ADDRESS_TYPE(AddressType) {
  NETWORK_ADDRESS_TYPE(IPV4),
  NETWORK_ADDRESS_TYPE(IPV6)
} TNetwork_Address_Type;

typedef struct NETWORK_ADDRESS_TYPE(TAddress) {
  TNetwork_Address_Type type;
  union {
    uint8_t ipv4[4];
    uint8_t ipv6[16];
  } bytes;
} TNetwork_Address;

typedef struct NETWORK_ADDRESS_TYPE(TEndpoint) {
  TNetwork_Address address;
  uint16_t port;
} TNetwork_Endpoint;

static inline void NETWORK_ADDRESS_FUNC(IPv4_Create)(
    TNetwork_Address *address, uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
  address->type = NETWORK_ADDRESS_TYPE(IPV4);
  address->bytes.ipv4[0] = a;
  address->bytes.ipv4[1] = b;
  address->bytes.ipv4[2] = c;
  address->bytes.ipv4[3] = d;
}

static inline void NETWORK_ADDRESS_FUNC(IPv4_Any)(TNetwork_Address *address) {
  NETWORK_ADDRESS_FUNC(IPv4_Create)(address, 0, 0, 0, 0);
}

static inline void NETWORK_ADDRESS_FUNC(IPv4_Loopback)(TNetwork_Address *address) {
  NETWORK_ADDRESS_FUNC(IPv4_Create)(address, 127, 0, 0, 1);
}

static inline void NETWORK_ADDRESS_FUNC(IPv6_Any)(TNetwork_Address *address) {
  address->type = NETWORK_ADDRESS_TYPE(IPV6);
  memset(address->bytes.ipv6, 0, sizeof(address->bytes.ipv6));
}

static inline void NETWORK_ADDRESS_FUNC(IPv6_Loopback)(TNetwork_Address *address) {
  NETWORK_ADDRESS_FUNC(IPv6_Any)(address);
  address->bytes.ipv6[15] = 1;
}

static inline void NETWORK_ADDRESS_FUNC(Endpoint_Create)(
    TNetwork_Endpoint *endpoint, const TNetwork_Address *address, uint16_t port) {
  endpoint->address = *address;
  endpoint->port = port;
}

static inline bool NETWORK_ADDRESS_FUNC(Equal)(
    const TNetwork_Address *left, const TNetwork_Address *right) {
  if (left->type != right->type)
    return false;
  return left->type == NETWORK_ADDRESS_TYPE(IPV4)
             ? memcmp(left->bytes.ipv4, right->bytes.ipv4, 4) == 0
             : memcmp(left->bytes.ipv6, right->bytes.ipv6, 16) == 0;
}
