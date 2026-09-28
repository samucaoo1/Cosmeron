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
  NETWORK_CONNECTION_CONST(IDLE),
  NETWORK_CONNECTION_CONST(CONNECTING),
  NETWORK_CONNECTION_CONST(CONNECTED),
  NETWORK_CONNECTION_CONST(CLOSING),
  NETWORK_CONNECTION_CONST(CLOSED),
  NETWORK_CONNECTION_CONST(FAILED)
} NETWORK_CONNECTION_TYPE(State);

typedef struct NETWORK_CONNECTION_TYPE(TConnection) {
  NETWORK_CONNECTION_TYPE(State) state;
  NETWORK_TYPE(Error) lastError;
  NETWORK_ADDRESS_TYPE(TEndpoint) localEndpoint;
  NETWORK_ADDRESS_TYPE(TEndpoint) remoteEndpoint;
  char *host;
  uint16_t port;
  bool blocking;
  CHRONOMETRY_TYPE(Duration) connectTimeout;
  SOCKET_TYPE(TSocket) socket;
} NETWORK_CONNECTION_TYPE(TConnection);

static inline void NETWORK_CONNECTION_FUNC(Init)(NETWORK_CONNECTION_TYPE(TConnection) *connection);
static inline OPSTATUS NETWORK_CONNECTION_FUNC(Connect)(
    NETWORK_CONNECTION_TYPE(TConnection) *connection, const char *host, uint16_t port,
    NETWORK_TYPE(Error) *error);
static inline OPSTATUS NETWORK_CONNECTION_FUNC(ConnectFor)(
    NETWORK_CONNECTION_TYPE(TConnection) *connection, const char *host, uint16_t port,
    CHRONOMETRY_TYPE(Duration) timeout, NETWORK_TYPE(Error) *error);
static inline OPSTATUS NETWORK_CONNECTION_FUNC(Reconnect)(
    NETWORK_CONNECTION_TYPE(TConnection) *connection, NETWORK_TYPE(Error) *error);
static inline OPSTATUS NETWORK_CONNECTION_FUNC(Read)(
    NETWORK_CONNECTION_TYPE(TConnection) *connection, void *buffer, size_t capacity,
    size_t *bytesRead, NETWORK_TYPE(Error) *error);
static inline OPSTATUS NETWORK_CONNECTION_FUNC(ReadExact)(
    NETWORK_CONNECTION_TYPE(TConnection) *connection, void *buffer, size_t size,
    NETWORK_TYPE(Error) *error);
static inline OPSTATUS NETWORK_CONNECTION_FUNC(Write)(
    NETWORK_CONNECTION_TYPE(TConnection) *connection, const void *data, size_t size,
    size_t *bytesWritten, NETWORK_TYPE(Error) *error);
static inline OPSTATUS NETWORK_CONNECTION_FUNC(WriteAll)(
    NETWORK_CONNECTION_TYPE(TConnection) *connection, const void *data, size_t size,
    NETWORK_TYPE(Error) *error);
static inline OPSTATUS NETWORK_CONNECTION_FUNC(Shutdown)(
    NETWORK_CONNECTION_TYPE(TConnection) *connection, SOCKET_TYPE(Shutdown) direction,
    NETWORK_TYPE(Error) *error);
static inline OPSTATUS NETWORK_CONNECTION_FUNC(SetBlocking)(
    NETWORK_CONNECTION_TYPE(TConnection) *connection, bool blocking, NETWORK_TYPE(Error) *error);
static inline void NETWORK_CONNECTION_FUNC(Close)(NETWORK_CONNECTION_TYPE(TConnection) *connection);
static inline void NETWORK_CONNECTION_FUNC(Destroy)(NETWORK_CONNECTION_TYPE(TConnection) *connection);

static inline NETWORK_CONNECTION_TYPE(State)
NETWORK_CONNECTION_FUNC(State)(const NETWORK_CONNECTION_TYPE(TConnection) *connection) {
  return connection->state;
}
static inline NETWORK_TYPE(Error)
NETWORK_CONNECTION_FUNC(LastError)(const NETWORK_CONNECTION_TYPE(TConnection) *connection) {
  return connection->lastError;
}
static inline const NETWORK_ADDRESS_TYPE(TEndpoint) *
NETWORK_CONNECTION_FUNC(LocalEndpoint)(const NETWORK_CONNECTION_TYPE(TConnection) *connection) {
  return &connection->localEndpoint;
}
static inline const NETWORK_ADDRESS_TYPE(TEndpoint) *
NETWORK_CONNECTION_FUNC(RemoteEndpoint)(const NETWORK_CONNECTION_TYPE(TConnection) *connection) {
  return &connection->remoteEndpoint;
}

#include "Impl/Connection.impl"
