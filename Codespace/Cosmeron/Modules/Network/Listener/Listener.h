#pragma once

#include "../Connection/Connection.h"

#define NETWORK_LISTENER_DEFAULT_BACKLOG 128

typedef struct NETWORK_LISTENER_TYPE(TListener) {
  TNetwork_Error lastError;
  TNetwork_Endpoint localEndpoint;
  bool blocking;
  bool dualStack;
  TSocket primary;
  TSocket secondary;
} TNetwork_Listener;

inline static void NETWORK_LISTENER_FUNC(Init)(TNetwork_Listener *listener);
inline static OPSTATUS NETWORK_LISTENER_FUNC(Listen)(
    TNetwork_Listener *listener, uint16_t port, TNetwork_Error *error);
inline static OPSTATUS NETWORK_LISTENER_FUNC(ListenAt)(
    TNetwork_Listener *listener, const TNetwork_Endpoint *endpoint,
    TNetwork_Error *error);
inline static OPSTATUS NETWORK_LISTENER_FUNC(Accept)(
    TNetwork_Listener *listener, TNetwork_Connection *connection,
    TNetwork_Error *error);
inline static void NETWORK_LISTENER_FUNC(Close)(TNetwork_Listener *listener);

static inline TNetwork_Error
NETWORK_LISTENER_FUNC(LastError)(const TNetwork_Listener *listener) {
  return listener->lastError;
}
static inline const TNetwork_Endpoint *
NETWORK_LISTENER_FUNC(LocalEndpoint)(const TNetwork_Listener *listener) {
  return &listener->localEndpoint;
}

#include "Impl/Listener.impl"
