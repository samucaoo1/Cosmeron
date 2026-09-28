#pragma once

#include "../Address/Address.h"
#include "../Error/Error.h"
#include "../Socket/Socket.h"
#include "../../../Core/Error/Status.h"

typedef struct NETWORK_DATAGRAM_TYPE(TDatagram) {
  TNetwork_Error lastError;
  TNetwork_Endpoint localEndpoint;
  TNetwork_Endpoint peerEndpoint;
  bool blocking;
  bool hasPeer;
  TSocket socket;
} TNetwork_Datagram;

inline static void NETWORK_DATAGRAM_FUNC(Init)(TNetwork_Datagram *datagram);
inline static OPSTATUS NETWORK_DATAGRAM_FUNC(Open)(
    TNetwork_Datagram *datagram, TSocket_Family family,
    TNetwork_Error *error);
inline static OPSTATUS NETWORK_DATAGRAM_FUNC(Bind)(
    TNetwork_Datagram *datagram, const TNetwork_Endpoint *endpoint,
    TNetwork_Error *error);
inline static OPSTATUS NETWORK_DATAGRAM_FUNC(SetPeer)(
    TNetwork_Datagram *datagram, const TNetwork_Endpoint *endpoint,
    TNetwork_Error *error);
inline static OPSTATUS NETWORK_DATAGRAM_FUNC(SendTo)(
    TNetwork_Datagram *datagram, const TNetwork_Endpoint *endpoint,
    const void *data, size_t size, size_t *sent, TNetwork_Error *error);
inline static OPSTATUS NETWORK_DATAGRAM_FUNC(ReceiveFrom)(
    TNetwork_Datagram *datagram, void *buffer, size_t capacity,
    TNetwork_Endpoint *sender, size_t *received, TNetwork_Error *error);
inline static OPSTATUS NETWORK_DATAGRAM_FUNC(Send)(
    TNetwork_Datagram *datagram, const void *data, size_t size, size_t *sent,
    TNetwork_Error *error);
inline static OPSTATUS NETWORK_DATAGRAM_FUNC(Receive)(
    TNetwork_Datagram *datagram, void *buffer, size_t capacity,
    size_t *received, TNetwork_Error *error);
inline static OPSTATUS NETWORK_DATAGRAM_FUNC(SetBlocking)(
    TNetwork_Datagram *datagram, bool blocking, TNetwork_Error *error);
inline static OPSTATUS NETWORK_DATAGRAM_FUNC(ClearPeer)(
    TNetwork_Datagram *datagram, TNetwork_Error *error);
inline static void NETWORK_DATAGRAM_FUNC(Close)(TNetwork_Datagram *datagram);

static inline bool
NETWORK_DATAGRAM_FUNC(HasPeer)(const TNetwork_Datagram *datagram) {
  return datagram->hasPeer;
}
static inline TNetwork_Error
NETWORK_DATAGRAM_FUNC(LastError)(const TNetwork_Datagram *datagram) {
  return datagram->lastError;
}
static inline const TNetwork_Endpoint *
NETWORK_DATAGRAM_FUNC(LocalEndpoint)(const TNetwork_Datagram *datagram) {
  return &datagram->localEndpoint;
}
static inline const TNetwork_Endpoint *
NETWORK_DATAGRAM_FUNC(PeerEndpoint)(const TNetwork_Datagram *datagram) {
  return datagram->hasPeer ? &datagram->peerEndpoint : NULL;
}

#include "Impl/Datagram.impl"
