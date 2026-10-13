#pragma once

#include "../Address/Address.h"
#include "../Error/Error.h"
#include "../Network.space"
#include "../Runtime.inc"
#include "../../../Core/Error/Status.h"
#include <limits.h>
#include <string.h>

#include "Impl/OS.inc"

typedef enum SOCKET_TYPE(TFamily) {
  SOCKET_CONST(FAMILY_IPV4),
  SOCKET_CONST(FAMILY_IPV6)
} SOCKET_TYPE(TFamily);

typedef enum SOCKET_TYPE(TKind) {
  SOCKET_CONST(KIND_STREAM),
  SOCKET_CONST(KIND_DATAGRAM)
} SOCKET_TYPE(TKind);

typedef enum SOCKET_TYPE(TProtocol) {
  SOCKET_CONST(PROTOCOL_DEFAULT),
  SOCKET_CONST(PROTOCOL_TCP),
  SOCKET_CONST(PROTOCOL_UDP)
} SOCKET_TYPE(TProtocol);

typedef enum SOCKET_TYPE(TShutdown) {
  SOCKET_CONST(SHUTDOWN_READ),
  SOCKET_CONST(SHUTDOWN_WRITE),
  SOCKET_CONST(SHUTDOWN_BOTH)
} SOCKET_TYPE(TShutdown);

typedef struct SOCKET_TYPE(TSocket) {
  SOCKET_TYPE(Handle) handle;
  SOCKET_TYPE(TFamily) family;
  SOCKET_TYPE(TKind) kind;
  SOCKET_TYPE(TProtocol) protocol;
  bool blocking;
} SOCKET_TYPE(TSocket);

static inline OPSTATUS SOCKET_INS(Return)(
    NETWORK_TYPE(TError) *error, NETWORK_TYPE(TError) domainError,
    OPSTATUS status) {
  if (error != NULL)
    *error = domainError;
  return status;
}

#define SOCKET_INIT_PROTOTYPE                                                  \
  static inline void SOCKET_FUNC(Init)(SOCKET_TYPE(TSocket) *socket)
#define SOCKET_CREATE_PROTOTYPE                                                \
  static inline OPSTATUS SOCKET_FUNC(Create)(                                  \
      SOCKET_TYPE(TSocket) *socket, SOCKET_TYPE(TFamily) family, SOCKET_TYPE(TKind) kind,                \
      SOCKET_TYPE(TProtocol) protocol, NETWORK_TYPE(TError) *error)
#define SOCKET_CLOSE_PROTOTYPE                                                 \
  static inline void SOCKET_FUNC(Close)(SOCKET_TYPE(TSocket) *socket)
#define SOCKET_BIND_PROTOTYPE                                                  \
  static inline OPSTATUS SOCKET_FUNC(Bind)(                                    \
      SOCKET_TYPE(TSocket) *socket, const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint, NETWORK_TYPE(TError) *error)
#define SOCKET_LISTEN_PROTOTYPE                                                \
  static inline OPSTATUS SOCKET_FUNC(Listen)(                                  \
      SOCKET_TYPE(TSocket) *socket, int backlog, NETWORK_TYPE(TError) *error)
#define SOCKET_ACCEPT_PROTOTYPE                                                \
  static inline OPSTATUS SOCKET_FUNC(Accept)(                                  \
      SOCKET_TYPE(TSocket) *socket, SOCKET_TYPE(TSocket) *client, NETWORK_ADDRESS_TYPE(TEndpoint) *peer,                \
      NETWORK_TYPE(TError) *error)
#define SOCKET_CONNECT_PROTOTYPE                                               \
  static inline OPSTATUS SOCKET_FUNC(Connect)(                                 \
      SOCKET_TYPE(TSocket) *socket, const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint, NETWORK_TYPE(TError) *error)
#define SOCKET_SEND_PROTOTYPE                                                  \
  static inline OPSTATUS SOCKET_FUNC(Send)(                                    \
      SOCKET_TYPE(TSocket) *socket, const void *data, size_t size, size_t *sent,             \
      NETWORK_TYPE(TError) *error)
#define SOCKET_RECEIVE_PROTOTYPE                                               \
  static inline OPSTATUS SOCKET_FUNC(Receive)(                                 \
      SOCKET_TYPE(TSocket) *socket, void *data, size_t capacity, size_t *received,           \
      NETWORK_TYPE(TError) *error)
#define SOCKET_SEND_TO_PROTOTYPE                                               \
  static inline OPSTATUS SOCKET_FUNC(SendTo)(                                  \
      SOCKET_TYPE(TSocket) *socket, const void *data, size_t size,                           \
      const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint, size_t *sent, NETWORK_TYPE(TError) *error)
#define SOCKET_RECEIVE_FROM_PROTOTYPE                                          \
  static inline OPSTATUS SOCKET_FUNC(ReceiveFrom)(                             \
      SOCKET_TYPE(TSocket) *socket, void *data, size_t capacity, NETWORK_ADDRESS_TYPE(TEndpoint) *sender,  \
      size_t *received, NETWORK_TYPE(TError) *error)
#define SOCKET_SHUTDOWN_PROTOTYPE                                              \
  static inline OPSTATUS SOCKET_FUNC(Shutdown)(                                \
      SOCKET_TYPE(TSocket) *socket, SOCKET_TYPE(TShutdown) direction, NETWORK_TYPE(TError) *error)
#define SOCKET_SET_BLOCKING_PROTOTYPE                                          \
  static inline OPSTATUS SOCKET_FUNC(SetBlocking)(                             \
      SOCKET_TYPE(TSocket) *socket, bool blocking, NETWORK_TYPE(TError) *error)
#define SOCKET_SET_REUSE_ADDRESS_PROTOTYPE                                     \
  static inline OPSTATUS SOCKET_FUNC(SetReuseAddress)(                         \
      SOCKET_TYPE(TSocket) *socket, bool enabled, NETWORK_TYPE(TError) *error)
#define SOCKET_SET_KEEP_ALIVE_PROTOTYPE                                        \
  static inline OPSTATUS SOCKET_FUNC(SetKeepAlive)(                            \
      SOCKET_TYPE(TSocket) *socket, bool enabled, NETWORK_TYPE(TError) *error)
#define SOCKET_SET_NO_DELAY_PROTOTYPE                                          \
  static inline OPSTATUS SOCKET_FUNC(SetNoDelay)(                              \
      SOCKET_TYPE(TSocket) *socket, bool enabled, NETWORK_TYPE(TError) *error)

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
