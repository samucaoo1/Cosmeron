#pragma once

#include "../Address/Address.h"
#include "../Error/Error.h"
#include "../Runtime.inc"
#include "../../Container/Array/Vector.h"

#if OS_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>

#else
#include <netdb.h>
#include <sys/socket.h>
#endif

FLAT_VECTOR_IMPLEMENT_ALL(NETWORK_ADDRESS_TYPE(TAddress), Network_Address)

typedef FLAT_VECTOR_TYPE(Network_Address) NETWORK_RESOLVE_TYPE(TAddressVector);

static inline NETWORK_TYPE(TError) NETWORK_INS(ResolveMapError)(int error) {
  (void)error;
#ifdef EAI_NONAME
  if (error == EAI_NONAME)
    return NETWORK_ERROR_CONST(RESOLVE_HOST_NOT_FOUND);
#endif
#ifdef EAI_AGAIN
  if (error == EAI_AGAIN)
    return NETWORK_ERROR_CONST(RESOURCE_EXHAUSTED);
#endif
#ifdef EAI_FAMILY
  if (error == EAI_FAMILY)
    return NETWORK_ERROR_CONST(ADDRESS_FAMILY_UNSUPPORTED);
#endif
  return NETWORK_ERROR_CONST(UNKNOWN);
}

static inline int NETWORK_INS(ResolveGetAddrInfo)(
    const char *host, const struct addrinfo *hints, struct addrinfo **result) {
#if OS_WINDOWS
  return NETWORK_FUNC(_WinGetAddrInfo)(host, NULL, hints, result);
#else
  return getaddrinfo(host, NULL, hints, result);
#endif
}

static inline void NETWORK_INS(ResolveFreeAddrInfo)(struct addrinfo *result) {
#if OS_WINDOWS
  NETWORK_FUNC(_WinFreeAddrInfo)(result);
#else
  freeaddrinfo(result);
#endif
}

#define NETWORK_RESOLVE_ADDRESS_PROTOTYPE                                    \
  static inline OPSTATUS NETWORK_RESOLVE_FUNC(Address)(                       \
      NETWORK_RESOLVE_TYPE(TAddressVector) *addresses, const char *host,      \
      NETWORK_TYPE(TError) *error)

NETWORK_RESOLVE_ADDRESS_PROTOTYPE;

NETWORK_RESOLVE_ADDRESS_PROTOTYPE {
  struct addrinfo hints;
  struct addrinfo *result = NULL;
  struct addrinfo *current = NULL;
  int status;

  if (addresses == NULL || host == NULL) {
    if (error != NULL)
      *error = NETWORK_ERROR_CONST(NONE);
    return STATUS_CONST(INVALID_ARGUMENT);
  }

  FLAT_VECTOR_FUNC(Network_Address, Clear)(addresses);
#if OS_WINDOWS
  if (!NETWORK_FUNC(_EnsureRuntime)()) {
    if (error != NULL)
      *error = NETWORK_ERROR_CONST(UNKNOWN);
    return STATUS_CONST(GENERIC_ERROR);
  }
#endif
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = 0;

  status = NETWORK_INS(ResolveGetAddrInfo)(host, &hints, &result);
  if (status != 0) {
    if (error != NULL)
      *error = NETWORK_INS(ResolveMapError)(status);
    return STATUS_CONST(GENERIC_ERROR);
  }

  for (current = result; current != NULL; current = current->ai_next) {
    NETWORK_ADDRESS_TYPE(TAddress) address = {0};
    bool supported = false;

    if (current->ai_family == AF_INET) {
      const struct sockaddr_in *native =
          (const struct sockaddr_in *)current->ai_addr;
      address.type = NETWORK_ADDRESS_CONST(IPV4);
      memcpy(address.bytes.ipv4, &native->sin_addr, 4);
      supported = true;
    } else if (current->ai_family == AF_INET6) {
      const struct sockaddr_in6 *native =
          (const struct sockaddr_in6 *)current->ai_addr;
      address.type = NETWORK_ADDRESS_CONST(IPV6);
      memcpy(address.bytes.ipv6, &native->sin6_addr, 16);
      supported = true;
    }

    if (supported) {
      bool duplicate = false;
      size_t index;
      for (index = 0; index < addresses->size; ++index) {
        if (NETWORK_ADDRESS_FUNC(Equal)(&addresses->data[index], &address)) {
          duplicate = true;
          break;
        }
      }
      if (!duplicate &&
          FLAT_VECTOR_FUNC(Network_Address, PushBack)(addresses, address) !=
              STATUS_CONST(SUCCESS)) {
        NETWORK_INS(ResolveFreeAddrInfo)(result);
        if (error != NULL)
          *error = NETWORK_ERROR_CONST(RESOURCE_EXHAUSTED);
        return STATUS_CONST(GENERIC_ERROR);
      }
    }
  }

  NETWORK_INS(ResolveFreeAddrInfo)(result);
  if (addresses->size == 0) {
    if (error != NULL)
      *error = NETWORK_ERROR_CONST(RESOLVE_HOST_NOT_FOUND);
    return STATUS_CONST(GENERIC_ERROR);
  }

  if (error != NULL)
    *error = NETWORK_ERROR_CONST(NONE);
  return STATUS_CONST(SUCCESS);
}
