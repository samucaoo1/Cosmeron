#pragma once

#include "../Network.space"
#include <string.h>

typedef enum NETWORK_ADDRESS_TYPE(TType) {
  NETWORK_ADDRESS_CONST(IPV4),
  NETWORK_ADDRESS_CONST(IPV6)
} NETWORK_ADDRESS_TYPE(TType);

typedef struct NETWORK_ADDRESS_TYPE(TAddress) {
  NETWORK_ADDRESS_TYPE(TType) type;
  union {
    uint8_t ipv4[4];
    uint8_t ipv6[16];
  } bytes;
} NETWORK_ADDRESS_TYPE(TAddress);

typedef struct NETWORK_ADDRESS_TYPE(TEndpoint) {
  NETWORK_ADDRESS_TYPE(TAddress) address;
  uint16_t port;
} NETWORK_ADDRESS_TYPE(TEndpoint);

#define NETWORK_ADDRESS_IPV4_CREATE_PROTOTYPE                               \
  static inline void NETWORK_ADDRESS_FUNC(IPv4_Create)(                       \
      NETWORK_ADDRESS_TYPE(TAddress) *address, uint8_t a, uint8_t b,          \
      uint8_t c, uint8_t d)

#define NETWORK_ADDRESS_IPV4_ANY_PROTOTYPE                                  \
  static inline void NETWORK_ADDRESS_FUNC(IPv4_Any)(                          \
      NETWORK_ADDRESS_TYPE(TAddress) *address)

#define NETWORK_ADDRESS_IPV4_LOOPBACK_PROTOTYPE                             \
  static inline void NETWORK_ADDRESS_FUNC(IPv4_Loopback)(                     \
      NETWORK_ADDRESS_TYPE(TAddress) *address)

#define NETWORK_ADDRESS_IPV6_ANY_PROTOTYPE                                  \
  static inline void NETWORK_ADDRESS_FUNC(IPv6_Any)(                          \
      NETWORK_ADDRESS_TYPE(TAddress) *address)

#define NETWORK_ADDRESS_IPV6_LOOPBACK_PROTOTYPE                             \
  static inline void NETWORK_ADDRESS_FUNC(IPv6_Loopback)(                     \
      NETWORK_ADDRESS_TYPE(TAddress) *address)

#define NETWORK_ADDRESS_ENDPOINT_CREATE_PROTOTYPE                           \
  static inline void NETWORK_ADDRESS_FUNC(Endpoint_Create)(                   \
      NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint,                              \
      const NETWORK_ADDRESS_TYPE(TAddress) *address, uint16_t port)

#define NETWORK_ADDRESS_EQUAL_PROTOTYPE                                     \
  static inline bool NETWORK_ADDRESS_FUNC(Equal)(                             \
      const NETWORK_ADDRESS_TYPE(TAddress) *left,                             \
      const NETWORK_ADDRESS_TYPE(TAddress) *right)

NETWORK_ADDRESS_IPV4_CREATE_PROTOTYPE {
  if (address == NULL)
    return;
  address->type = NETWORK_ADDRESS_CONST(IPV4);
  address->bytes.ipv4[0] = a;
  address->bytes.ipv4[1] = b;
  address->bytes.ipv4[2] = c;
  address->bytes.ipv4[3] = d;
}

NETWORK_ADDRESS_IPV4_ANY_PROTOTYPE {
  NETWORK_ADDRESS_FUNC(IPv4_Create)(address, 0, 0, 0, 0);
}

NETWORK_ADDRESS_IPV4_LOOPBACK_PROTOTYPE {
  NETWORK_ADDRESS_FUNC(IPv4_Create)(address, 127, 0, 0, 1);
}

NETWORK_ADDRESS_IPV6_ANY_PROTOTYPE {
  if (address == NULL)
    return;
  address->type = NETWORK_ADDRESS_CONST(IPV6);
  memset(address->bytes.ipv6, 0, sizeof(address->bytes.ipv6));
}

NETWORK_ADDRESS_IPV6_LOOPBACK_PROTOTYPE {
  if (address == NULL)
    return;
  NETWORK_ADDRESS_FUNC(IPv6_Any)(address);
  address->bytes.ipv6[15] = 1;
}

NETWORK_ADDRESS_ENDPOINT_CREATE_PROTOTYPE {
  if (endpoint == NULL || address == NULL)
    return;
  endpoint->address = *address;
  endpoint->port = port;
}

NETWORK_ADDRESS_EQUAL_PROTOTYPE {
  if (left == NULL || right == NULL)
    return false;
  if (left->type != right->type)
    return false;
  return left->type == NETWORK_ADDRESS_CONST(IPV4)
             ? memcmp(left->bytes.ipv4, right->bytes.ipv4, 4) == 0
             : memcmp(left->bytes.ipv6, right->bytes.ipv6, 16) == 0;
}
