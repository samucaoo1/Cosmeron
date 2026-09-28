#include <assert.h>

#include "../../Cosmeron/Modules/Network/Socket/Socket.h"

int main(void) {
  TSocket socket;
  SOCKET_TYPE(Handle) first;

  SOCKET_FUNC(Init)(&socket);
  assert(SOCKET_FUNC(Create)(&socket, SOCKET_TYPE(FAMILY_IPV4),
                             SOCKET_TYPE(KIND_DATAGRAM),
                             SOCKET_TYPE(PROTOCOL_UDP), NULL) == STATUS_NS(SUCCESS));
  first = socket.handle;
  assert(first != TSOCKET_NATIVE_INVALID);

  assert(SOCKET_FUNC(Create)(&socket, SOCKET_TYPE(FAMILY_IPV4),
                             SOCKET_TYPE(KIND_DATAGRAM),
                             SOCKET_TYPE(PROTOCOL_UDP), NULL) == STATUS_NS(SUCCESS));
  assert(socket.handle != TSOCKET_NATIVE_INVALID);

  SOCKET_FUNC(Close)(&socket);
  assert(socket.handle == TSOCKET_NATIVE_INVALID);
  return 0;
}
