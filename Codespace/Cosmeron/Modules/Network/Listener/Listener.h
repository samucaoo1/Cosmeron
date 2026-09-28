#pragma once

#include "../Connection/Connection.h"

#define NETWORK_LISTENER_DEFAULT_BACKLOG 128

typedef struct NETWORK_LISTENER_TYPE(TListener) {
  NETWORK_TYPE(Error) lastError;
  NETWORK_ADDRESS_TYPE(TEndpoint) localEndpoint;
  bool blocking;
  bool dualStack;
  SOCKET_TYPE(TSocket) primary;
  SOCKET_TYPE(TSocket) secondary;
} NETWORK_LISTENER_TYPE(TListener);

static inline void NETWORK_LISTENER_FUNC(Init)(NETWORK_LISTENER_TYPE(TListener) *listener);
static inline OPSTATUS NETWORK_LISTENER_FUNC(Listen)(
    NETWORK_LISTENER_TYPE(TListener) *listener, uint16_t port, NETWORK_TYPE(Error) *error);
static inline OPSTATUS NETWORK_LISTENER_FUNC(ListenAt)(
    NETWORK_LISTENER_TYPE(TListener) *listener, const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint,
    NETWORK_TYPE(Error) *error);
static inline OPSTATUS NETWORK_LISTENER_FUNC(Accept)(
    NETWORK_LISTENER_TYPE(TListener) *listener, NETWORK_CONNECTION_TYPE(TConnection) *connection,
    NETWORK_TYPE(Error) *error);
static inline void NETWORK_LISTENER_FUNC(Close)(NETWORK_LISTENER_TYPE(TListener) *listener);

static inline NETWORK_TYPE(Error)
NETWORK_LISTENER_FUNC(LastError)(const NETWORK_LISTENER_TYPE(TListener) *listener) {
  return listener->lastError;
}
static inline const NETWORK_ADDRESS_TYPE(TEndpoint) *
NETWORK_LISTENER_FUNC(LocalEndpoint)(const NETWORK_LISTENER_TYPE(TListener) *listener) {
  return &listener->localEndpoint;
}

#include "Impl/Listener.impl"
