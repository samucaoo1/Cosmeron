#pragma once

#include "../Address/Address.h"
#include "../Error/Error.h"
#include "../Socket/Socket.h"
#include "../../../Core/Error/Status.h"

typedef struct NETWORK_DATAGRAM_TYPE(TDatagram) {
  NETWORK_TYPE(TError) lastError;
  NETWORK_ADDRESS_TYPE(TEndpoint) localEndpoint;
  NETWORK_ADDRESS_TYPE(TEndpoint) peerEndpoint;
  bool blocking;
  bool hasPeer;
  SOCKET_TYPE(TSocket) socket;
} NETWORK_DATAGRAM_TYPE(TDatagram);

static inline void NETWORK_DATAGRAM_FUNC(Init)(NETWORK_DATAGRAM_TYPE(TDatagram) *datagram);
static inline OPSTATUS NETWORK_DATAGRAM_FUNC(Open)(
    NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, SOCKET_TYPE(TFamily) family,
    NETWORK_TYPE(TError) *error);
static inline OPSTATUS NETWORK_DATAGRAM_FUNC(Bind)(
    NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint,
    NETWORK_TYPE(TError) *error);
static inline OPSTATUS NETWORK_DATAGRAM_FUNC(SetPeer)(
    NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint,
    NETWORK_TYPE(TError) *error);
static inline OPSTATUS NETWORK_DATAGRAM_FUNC(SendTo)(
    NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint,
    const void *data, size_t size, size_t *sent, NETWORK_TYPE(TError) *error);
static inline OPSTATUS NETWORK_DATAGRAM_FUNC(ReceiveFrom)(
    NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, void *buffer, size_t capacity,
    NETWORK_ADDRESS_TYPE(TEndpoint) *sender, size_t *received, NETWORK_TYPE(TError) *error);
static inline OPSTATUS NETWORK_DATAGRAM_FUNC(Send)(
    NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, const void *data, size_t size, size_t *sent,
    NETWORK_TYPE(TError) *error);
static inline OPSTATUS NETWORK_DATAGRAM_FUNC(Receive)(
    NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, void *buffer, size_t capacity,
    size_t *received, NETWORK_TYPE(TError) *error);
static inline OPSTATUS NETWORK_DATAGRAM_FUNC(SetBlocking)(
    NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, bool blocking, NETWORK_TYPE(TError) *error);
static inline OPSTATUS NETWORK_DATAGRAM_FUNC(ClearPeer)(
    NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, NETWORK_TYPE(TError) *error);
static inline void NETWORK_DATAGRAM_FUNC(Close)(NETWORK_DATAGRAM_TYPE(TDatagram) *datagram);

static inline bool
NETWORK_DATAGRAM_FUNC(HasPeer)(const NETWORK_DATAGRAM_TYPE(TDatagram) *datagram) {
  return datagram->hasPeer;
}
static inline NETWORK_TYPE(TError)
NETWORK_DATAGRAM_FUNC(LastError)(const NETWORK_DATAGRAM_TYPE(TDatagram) *datagram) {
  return datagram->lastError;
}
static inline const NETWORK_ADDRESS_TYPE(TEndpoint) *
NETWORK_DATAGRAM_FUNC(LocalEndpoint)(const NETWORK_DATAGRAM_TYPE(TDatagram) *datagram) {
  return &datagram->localEndpoint;
}
static inline const NETWORK_ADDRESS_TYPE(TEndpoint) *
NETWORK_DATAGRAM_FUNC(PeerEndpoint)(const NETWORK_DATAGRAM_TYPE(TDatagram) *datagram) {
  return datagram->hasPeer ? &datagram->peerEndpoint : NULL;
}

#include "Impl/Datagram.impl"
