#pragma once

#include "../Address/Address.h"
#include "../Error/Error.h"
#include "../Network.space"
#include "../Runtime.inc"
#include "../../../Core/Error/Status.h"
#include <limits.h>
#include <string.h>

#include "Impl/OS.inc"

typedef enum SOCKET_TYPE(Family) {
  SOCKET_TYPE(FAMILY_IPV4),
  SOCKET_TYPE(FAMILY_IPV6)
} TSocket_Family;

typedef enum SOCKET_TYPE(Kind) {
  SOCKET_TYPE(KIND_STREAM),
  SOCKET_TYPE(KIND_DATAGRAM)
} TSocket_Kind;

typedef enum SOCKET_TYPE(Protocol) {
  SOCKET_TYPE(PROTOCOL_DEFAULT),
  SOCKET_TYPE(PROTOCOL_TCP),
  SOCKET_TYPE(PROTOCOL_UDP)
} TSocket_Protocol;

typedef enum SOCKET_TYPE(Shutdown) {
  SOCKET_TYPE(SHUTDOWN_READ),
  SOCKET_TYPE(SHUTDOWN_WRITE),
  SOCKET_TYPE(SHUTDOWN_BOTH)
} TSocket_Shutdown;

typedef struct SOCKET_TYPE(TSocket) {
  SOCKET_TYPE(Handle) handle;
  TSocket_Family family;
  TSocket_Kind kind;
  TSocket_Protocol protocol;
  TNetwork_Error lastError;
  bool blocking;
} TSocket;

static inline OPSTATUS SOCKET_FUNC(_Return)(
    TSocket *socket, TNetwork_Error *error, TNetwork_Error domainError,
    OPSTATUS status) {
  if (socket != NULL)
    socket->lastError = domainError;
  if (error != NULL)
    *error = domainError;
  return status;
}

#define SOCKET_INIT_PROTOTYPE                                                  \
  inline static void SOCKET_FUNC(Init)(TSocket *socket)
#define SOCKET_CREATE_PROTOTYPE                                                \
  inline static OPSTATUS SOCKET_FUNC(Create)(                                  \
      TSocket *socket, TSocket_Family family, TSocket_Kind kind,                \
      TSocket_Protocol protocol, TNetwork_Error *error)
#define SOCKET_CLOSE_PROTOTYPE                                                 \
  inline static void SOCKET_FUNC(Close)(TSocket *socket)
#define SOCKET_BIND_PROTOTYPE                                                  \
  inline static OPSTATUS SOCKET_FUNC(Bind)(                                    \
      TSocket *socket, const TNetwork_Endpoint *endpoint, TNetwork_Error *error)
#define SOCKET_LISTEN_PROTOTYPE                                                \
  inline static OPSTATUS SOCKET_FUNC(Listen)(                                  \
      TSocket *socket, int backlog, TNetwork_Error *error)
#define SOCKET_ACCEPT_PROTOTYPE                                                \
  inline static OPSTATUS SOCKET_FUNC(Accept)(                                  \
      TSocket *socket, TSocket *client, TNetwork_Endpoint *peer,                \
      TNetwork_Error *error)
#define SOCKET_CONNECT_PROTOTYPE                                               \
  inline static OPSTATUS SOCKET_FUNC(Connect)(                                 \
      TSocket *socket, const TNetwork_Endpoint *endpoint, TNetwork_Error *error)
#define SOCKET_SEND_PROTOTYPE                                                  \
  inline static OPSTATUS SOCKET_FUNC(Send)(                                    \
      TSocket *socket, const void *data, size_t size, size_t *sent,             \
      TNetwork_Error *error)
#define SOCKET_RECEIVE_PROTOTYPE                                               \
  inline static OPSTATUS SOCKET_FUNC(Receive)(                                 \
      TSocket *socket, void *data, size_t capacity, size_t *received,           \
      TNetwork_Error *error)
#define SOCKET_SEND_TO_PROTOTYPE                                               \
  inline static OPSTATUS SOCKET_FUNC(SendTo)(                                  \
      TSocket *socket, const void *data, size_t size,                           \
      const TNetwork_Endpoint *endpoint, size_t *sent, TNetwork_Error *error)
#define SOCKET_RECEIVE_FROM_PROTOTYPE                                          \
  inline static OPSTATUS SOCKET_FUNC(ReceiveFrom)(                             \
      TSocket *socket, void *data, size_t capacity, TNetwork_Endpoint *sender,  \
      size_t *received, TNetwork_Error *error)
#define SOCKET_SHUTDOWN_PROTOTYPE                                              \
  inline static OPSTATUS SOCKET_FUNC(Shutdown)(                                \
      TSocket *socket, TSocket_Shutdown direction, TNetwork_Error *error)
#define SOCKET_SET_BLOCKING_PROTOTYPE                                          \
  inline static OPSTATUS SOCKET_FUNC(SetBlocking)(                             \
      TSocket *socket, bool blocking, TNetwork_Error *error)
#define SOCKET_SET_REUSE_ADDRESS_PROTOTYPE                                     \
  inline static OPSTATUS SOCKET_FUNC(SetReuseAddress)(                         \
      TSocket *socket, bool enabled, TNetwork_Error *error)
#define SOCKET_SET_KEEP_ALIVE_PROTOTYPE                                        \
  inline static OPSTATUS SOCKET_FUNC(SetKeepAlive)(                            \
      TSocket *socket, bool enabled, TNetwork_Error *error)
#define SOCKET_SET_NO_DELAY_PROTOTYPE                                          \
  inline static OPSTATUS SOCKET_FUNC(SetNoDelay)(                              \
      TSocket *socket, bool enabled, TNetwork_Error *error)

SOCKET_INIT_PROTOTYPE;
SOCKET_CREATE_PROTOTYPE;
SOCKET_CLOSE_PROTOTYPE;
SOCKET_BIND_PROTOTYPE;
SOCKET_LISTEN_PROTOTYPE;
SOCKET_ACCEPT_PROTOTYPE;
SOCKET_CONNECT_PROTOTYPE;
SOCKET_SEND_PROTOTYPE;
SOCKET_RECEIVE_PROTOTYPE;
SOCKET_SEND_TO_PROTOTYPE;
SOCKET_RECEIVE_FROM_PROTOTYPE;
SOCKET_SHUTDOWN_PROTOTYPE;
SOCKET_SET_BLOCKING_PROTOTYPE;
SOCKET_SET_REUSE_ADDRESS_PROTOTYPE;
SOCKET_SET_KEEP_ALIVE_PROTOTYPE;
SOCKET_SET_NO_DELAY_PROTOTYPE;

#include "Impl/Lifecycle.impl"
#include "Impl/Connection.impl"
#include "Impl/IO.impl"
#include "Impl/Settings.impl"
