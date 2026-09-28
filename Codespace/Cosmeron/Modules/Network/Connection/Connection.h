#pragma once

#include "../Address/Address.h"
#include "../Error/Error.h"
#include "../Resolve/Resolve.h"
#include "../Socket/Socket.h"
#include "../../Chronometry/Duration/Duration.h"
#include "../../Chronometry/Clock/Clock.h"
#include "../../../Core/Error/Status.h"
#include "../../../Core/Memory/Alloc.h"
#include <string.h>

#define NETWORK_CONNECTION_DEFAULT_TIMEOUT_SECONDS INT64_C(30)

typedef enum NETWORK_CONNECTION_TYPE(State) {
  NETWORK_CONNECTION_TYPE(IDLE),
  NETWORK_CONNECTION_TYPE(CONNECTING),
  NETWORK_CONNECTION_TYPE(CONNECTED),
  NETWORK_CONNECTION_TYPE(CLOSING),
  NETWORK_CONNECTION_TYPE(CLOSED),
  NETWORK_CONNECTION_TYPE(FAILED)
} TNetwork_Connection_State;

typedef struct NETWORK_CONNECTION_TYPE(TConnection) {
  TNetwork_Connection_State state;
  TNetwork_Error lastError;
  TNetwork_Endpoint localEndpoint;
  TNetwork_Endpoint remoteEndpoint;
  char *host;
  uint16_t port;
  bool blocking;
  CHRONOMETRY_TYPE(Duration) connectTimeout;
  TSocket socket;
} TNetwork_Connection;

inline static void NETWORK_CONNECTION_FUNC(Init)(TNetwork_Connection *connection);
inline static OPSTATUS NETWORK_CONNECTION_FUNC(Connect)(
    TNetwork_Connection *connection, const char *host, uint16_t port,
    TNetwork_Error *error);
inline static OPSTATUS NETWORK_CONNECTION_FUNC(ConnectFor)(
    TNetwork_Connection *connection, const char *host, uint16_t port,
    CHRONOMETRY_TYPE(Duration) timeout, TNetwork_Error *error);
inline static OPSTATUS NETWORK_CONNECTION_FUNC(Reconnect)(
    TNetwork_Connection *connection, TNetwork_Error *error);
inline static OPSTATUS NETWORK_CONNECTION_FUNC(Read)(
    TNetwork_Connection *connection, void *buffer, size_t capacity,
    size_t *bytesRead, TNetwork_Error *error);
inline static OPSTATUS NETWORK_CONNECTION_FUNC(ReadExact)(
    TNetwork_Connection *connection, void *buffer, size_t size,
    TNetwork_Error *error);
inline static OPSTATUS NETWORK_CONNECTION_FUNC(Write)(
    TNetwork_Connection *connection, const void *data, size_t size,
    size_t *bytesWritten, TNetwork_Error *error);
inline static OPSTATUS NETWORK_CONNECTION_FUNC(WriteAll)(
    TNetwork_Connection *connection, const void *data, size_t size,
    TNetwork_Error *error);
inline static OPSTATUS NETWORK_CONNECTION_FUNC(Shutdown)(
    TNetwork_Connection *connection, TSocket_Shutdown direction,
    TNetwork_Error *error);
inline static OPSTATUS NETWORK_CONNECTION_FUNC(SetBlocking)(
    TNetwork_Connection *connection, bool blocking, TNetwork_Error *error);
inline static void NETWORK_CONNECTION_FUNC(Close)(TNetwork_Connection *connection);
inline static void NETWORK_CONNECTION_FUNC(Destroy)(TNetwork_Connection *connection);

static inline TNetwork_Connection_State
NETWORK_CONNECTION_FUNC(State)(const TNetwork_Connection *connection) {
  return connection->state;
}
static inline TNetwork_Error
NETWORK_CONNECTION_FUNC(LastError)(const TNetwork_Connection *connection) {
  return connection->lastError;
}
static inline const TNetwork_Endpoint *
NETWORK_CONNECTION_FUNC(LocalEndpoint)(const TNetwork_Connection *connection) {
  return &connection->localEndpoint;
}
static inline const TNetwork_Endpoint *
NETWORK_CONNECTION_FUNC(RemoteEndpoint)(const TNetwork_Connection *connection) {
  return &connection->remoteEndpoint;
}

#include "Impl/Connection.impl"
