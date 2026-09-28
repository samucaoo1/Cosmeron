#pragma once

#include "../Connection/Connection.h"

#define NETWORK_LISTENER_DEFAULT_BACKLOG 128

typedef struct NETWORK_LISTENER_TYPE(TListener) {
  NETWORK_TYPE(TError) lastError;
  NETWORK_ADDRESS_TYPE(TEndpoint) localEndpoint;
  bool blocking;
  bool dualStack;
  SOCKET_TYPE(TSocket) primary;
  SOCKET_TYPE(TSocket) secondary;
} NETWORK_LISTENER_TYPE(TListener);

#define NETWORK_LISTENER_INIT_PROTOTYPE                                     \
  static inline void NETWORK_LISTENER_FUNC(Init)(                             \
      NETWORK_LISTENER_TYPE(TListener) *listener)

#define NETWORK_LISTENER_LISTEN_PROTOTYPE                                   \
  static inline OPSTATUS NETWORK_LISTENER_FUNC(Listen)(                       \
      NETWORK_LISTENER_TYPE(TListener) *listener, uint16_t port,             \
      NETWORK_TYPE(TError) *error)

#define NETWORK_LISTENER_LISTEN_AT_PROTOTYPE                                \
  static inline OPSTATUS NETWORK_LISTENER_FUNC(ListenAt)(                     \
      NETWORK_LISTENER_TYPE(TListener) *listener,                             \
      const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint,                       \
      NETWORK_TYPE(TError) *error)

#define NETWORK_LISTENER_ACCEPT_PROTOTYPE                                   \
  static inline OPSTATUS NETWORK_LISTENER_FUNC(Accept)(                       \
      NETWORK_LISTENER_TYPE(TListener) *listener,                             \
      NETWORK_CONNECTION_TYPE(TConnection) *connection,                     \
      NETWORK_TYPE(TError) *error)

#define NETWORK_LISTENER_CLOSE_PROTOTYPE                                    \
  static inline void NETWORK_LISTENER_FUNC(Close)(                            \
      NETWORK_LISTENER_TYPE(TListener) *listener)

#define NETWORK_LISTENER_LAST_ERROR_PROTOTYPE                               \
  static inline NETWORK_TYPE(TError) NETWORK_LISTENER_FUNC(LastError)(       \
      const NETWORK_LISTENER_TYPE(TListener) *listener)

#define NETWORK_LISTENER_LOCAL_ENDPOINT_PROTOTYPE                           \
  static inline const NETWORK_ADDRESS_TYPE(TEndpoint) *                      \
  NETWORK_LISTENER_FUNC(LocalEndpoint)(                                       \
      const NETWORK_LISTENER_TYPE(TListener) *listener)

NETWORK_LISTENER_INIT_PROTOTYPE;
NETWORK_LISTENER_LISTEN_PROTOTYPE;
NETWORK_LISTENER_LISTEN_AT_PROTOTYPE;
NETWORK_LISTENER_ACCEPT_PROTOTYPE;
NETWORK_LISTENER_CLOSE_PROTOTYPE;

NETWORK_LISTENER_LAST_ERROR_PROTOTYPE {
  return listener->lastError;
}

NETWORK_LISTENER_LOCAL_ENDPOINT_PROTOTYPE {
  return &listener->localEndpoint;
}

#include "Impl/Listener.impl"
