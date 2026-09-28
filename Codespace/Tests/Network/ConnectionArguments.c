#include <assert.h>

#include "../../Cosmeron/Modules/Network/Connection/Connection.h"

int main(void) {
  NETWORK_CONNECTION_TYPE(TConnection) connection;
  SOCKET_TYPE(TSocket) socket;
  size_t count = 123;
  CHRONOMETRY_TYPE(Duration) negative;

  NETWORK_CONNECTION_FUNC(Init)(&connection);
  SOCKET_FUNC(Init)(&socket);

  assert(SOCKET_FUNC(Create)(
             &socket, (SOCKET_TYPE(TFamily))99, SOCKET_CONST(KIND_STREAM),
             SOCKET_CONST(PROTOCOL_TCP), NULL) == STATUS_CONST(INVALID_ARGUMENT));
  assert(socket.handle == TSOCKET_NATIVE_INVALID);
  assert(SOCKET_FUNC(Create)(
             &socket, SOCKET_CONST(FAMILY_IPV4), SOCKET_CONST(KIND_STREAM),
             SOCKET_CONST(PROTOCOL_UDP), NULL) == STATUS_CONST(INVALID_ARGUMENT));
  assert(SOCKET_FUNC(Shutdown)(
             &socket, (SOCKET_TYPE(TShutdown))99, NULL) ==
         STATUS_CONST(INVALID_ARGUMENT));

  assert(NETWORK_CONNECTION_FUNC(Read)(NULL, NULL, 0, &count, NULL) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(NETWORK_CONNECTION_FUNC(Read)(&connection, NULL, 1, &count, NULL) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(NETWORK_CONNECTION_FUNC(ReadExact)(NULL, NULL, 0, NULL) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(NETWORK_CONNECTION_FUNC(Write)(NULL, NULL, 0, &count, NULL) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(NETWORK_CONNECTION_FUNC(Write)(&connection, NULL, 1, &count, NULL) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(NETWORK_CONNECTION_FUNC(WriteAll)(NULL, NULL, 0, NULL) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(NETWORK_CONNECTION_FUNC(Shutdown)(
             NULL, SOCKET_CONST(SHUTDOWN_BOTH), NULL) == STATUS_CONST(INVALID_ARGUMENT));

  negative = DURATION_FUNC(FromNanoseconds)(-1);
  assert(NETWORK_CONNECTION_FUNC(ConnectFor)(
             &connection, "127.0.0.1", 1, negative, NULL) ==
         STATUS_CONST(INVALID_ARGUMENT));

  NETWORK_CONNECTION_FUNC(Destroy)(&connection);
  return 0;
}
