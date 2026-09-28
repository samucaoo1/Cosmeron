#pragma once

#include "../Address/Address.h"
#include "../Error/Error.h"
#include "../Socket/Socket.h"
#include "../../../Core/Error/Status.h"

typedef struct NETWORK_DATAGRAM_TYPE(TDatagram) {
  NETWORK_ADDRESS_TYPE(TEndpoint) localEndpoint;
  NETWORK_ADDRESS_TYPE(TEndpoint) peerEndpoint;
  bool blocking;
  bool hasPeer;
  SOCKET_TYPE(TSocket) socket;
} NETWORK_DATAGRAM_TYPE(TDatagram);

#define NETWORK_DATAGRAM_INIT_PROTOTYPE                                     \
  static inline void NETWORK_DATAGRAM_FUNC(Init)(                             \
      NETWORK_DATAGRAM_TYPE(TDatagram) *datagram)

#define NETWORK_DATAGRAM_OPEN_PROTOTYPE                                     \
  static inline OPSTATUS NETWORK_DATAGRAM_FUNC(Open)(                         \
      NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, SOCKET_TYPE(TFamily) family,\
      NETWORK_TYPE(TError) *error)

#define NETWORK_DATAGRAM_BIND_PROTOTYPE                                     \
  static inline OPSTATUS NETWORK_DATAGRAM_FUNC(Bind)(                         \
      NETWORK_DATAGRAM_TYPE(TDatagram) *datagram,                             \
      const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint,                       \
      NETWORK_TYPE(TError) *error)

#define NETWORK_DATAGRAM_SET_PEER_PROTOTYPE                                 \
  static inline OPSTATUS NETWORK_DATAGRAM_FUNC(SetPeer)(                      \
      NETWORK_DATAGRAM_TYPE(TDatagram) *datagram,                             \
      const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint,                       \
      NETWORK_TYPE(TError) *error)

#define NETWORK_DATAGRAM_SEND_TO_PROTOTYPE                                  \
  static inline OPSTATUS NETWORK_DATAGRAM_FUNC(SendTo)(                       \
      NETWORK_DATAGRAM_TYPE(TDatagram) *datagram,                             \
      const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint, const void *data,     \
      size_t size, size_t *sent, NETWORK_TYPE(TError) *error)

#define NETWORK_DATAGRAM_RECEIVE_FROM_PROTOTYPE                             \
  static inline OPSTATUS NETWORK_DATAGRAM_FUNC(ReceiveFrom)(                  \
      NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, void *buffer,              \
      size_t capacity, NETWORK_ADDRESS_TYPE(TEndpoint) *sender,              \
      size_t *received, NETWORK_TYPE(TError) *error)

#define NETWORK_DATAGRAM_SEND_PROTOTYPE                                     \
  static inline OPSTATUS NETWORK_DATAGRAM_FUNC(Send)(                         \
      NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, const void *data,          \
      size_t size, size_t *sent, NETWORK_TYPE(TError) *error)

#define NETWORK_DATAGRAM_RECEIVE_PROTOTYPE                                  \
  static inline OPSTATUS NETWORK_DATAGRAM_FUNC(Receive)(                      \
      NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, void *buffer,              \
      size_t capacity, size_t *received, NETWORK_TYPE(TError) *error)

#define NETWORK_DATAGRAM_SET_BLOCKING_PROTOTYPE                             \
  static inline OPSTATUS NETWORK_DATAGRAM_FUNC(SetBlocking)(                  \
      NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, bool blocking,             \
      NETWORK_TYPE(TError) *error)

#define NETWORK_DATAGRAM_CLEAR_PEER_PROTOTYPE                               \
  static inline OPSTATUS NETWORK_DATAGRAM_FUNC(ClearPeer)(                    \
      NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, NETWORK_TYPE(TError) *error)

#define NETWORK_DATAGRAM_CLOSE_PROTOTYPE                                    \
  static inline void NETWORK_DATAGRAM_FUNC(Close)(                            \
      NETWORK_DATAGRAM_TYPE(TDatagram) *datagram)

#define NETWORK_DATAGRAM_HAS_PEER_PROTOTYPE                                 \
  static inline bool NETWORK_DATAGRAM_FUNC(HasPeer)(                          \
      const NETWORK_DATAGRAM_TYPE(TDatagram) *datagram)

#define NETWORK_DATAGRAM_LOCAL_ENDPOINT_PROTOTYPE                           \
  static inline const NETWORK_ADDRESS_TYPE(TEndpoint) *                      \
  NETWORK_DATAGRAM_FUNC(LocalEndpoint)(                                       \
      const NETWORK_DATAGRAM_TYPE(TDatagram) *datagram)

#define NETWORK_DATAGRAM_PEER_ENDPOINT_PROTOTYPE                            \
  static inline const NETWORK_ADDRESS_TYPE(TEndpoint) *                      \
  NETWORK_DATAGRAM_FUNC(PeerEndpoint)(                                        \
      const NETWORK_DATAGRAM_TYPE(TDatagram) *datagram)

NETWORK_DATAGRAM_INIT_PROTOTYPE;
NETWORK_DATAGRAM_OPEN_PROTOTYPE;
NETWORK_DATAGRAM_BIND_PROTOTYPE;
NETWORK_DATAGRAM_SET_PEER_PROTOTYPE;
NETWORK_DATAGRAM_SEND_TO_PROTOTYPE;
NETWORK_DATAGRAM_RECEIVE_FROM_PROTOTYPE;
NETWORK_DATAGRAM_SEND_PROTOTYPE;
NETWORK_DATAGRAM_RECEIVE_PROTOTYPE;
NETWORK_DATAGRAM_SET_BLOCKING_PROTOTYPE;
NETWORK_DATAGRAM_CLEAR_PEER_PROTOTYPE;
NETWORK_DATAGRAM_CLOSE_PROTOTYPE;

NETWORK_DATAGRAM_HAS_PEER_PROTOTYPE {
  return datagram->hasPeer;
}


NETWORK_DATAGRAM_LOCAL_ENDPOINT_PROTOTYPE {
  return &datagram->localEndpoint;
}

NETWORK_DATAGRAM_PEER_ENDPOINT_PROTOTYPE {
  return datagram->hasPeer ? &datagram->peerEndpoint : NULL;
}

#include "Impl/Datagram.impl"
