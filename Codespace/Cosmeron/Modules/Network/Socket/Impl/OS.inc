#pragma once

#if OS_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET SOCKET_TYPE(Handle);
typedef int SOCKET_TYPE(Len);
#define TSOCKET_NATIVE_INVALID INVALID_SOCKET
#define TSOCKET_NATIVE_ERROR SOCKET_ERROR
#define TSOCKET_OS_CLOSE(h) NETWORK_FUNC(_WinCloseSocket)(h)
#define TSOCKET_NATIVE_ERROR_CODE() NETWORK_FUNC(_WinGetLastError)()
#define TSOCKET_NATIVE_HTONS(value) NETWORK_FUNC(_WinHostToNetworkShort)(value)
#define TSOCKET_NATIVE_NTOHS(value) NETWORK_FUNC(_WinNetworkToHostShort)(value)
#define TSOCKET_NATIVE_SOCKET NETWORK_FUNC(_WinSocket)
#define TSOCKET_NATIVE_BIND NETWORK_FUNC(_WinBind)
#define TSOCKET_NATIVE_LISTEN NETWORK_FUNC(_WinListen)
#define TSOCKET_NATIVE_ACCEPT NETWORK_FUNC(_WinAccept)
#define TSOCKET_NATIVE_CONNECT NETWORK_FUNC(_WinConnect)
#define TSOCKET_NATIVE_SEND NETWORK_FUNC(_WinSend)
#define TSOCKET_NATIVE_RECV NETWORK_FUNC(_WinReceive)
#define TSOCKET_NATIVE_SENDTO NETWORK_FUNC(_WinSendTo)
#define TSOCKET_NATIVE_RECVFROM NETWORK_FUNC(_WinReceiveFrom)
#define TSOCKET_NATIVE_SHUTDOWN NETWORK_FUNC(_WinShutdown)
#define TSOCKET_NATIVE_SETSOCKOPT NETWORK_FUNC(_WinSetSockOpt)
#define TSOCKET_NATIVE_GETSOCKOPT NETWORK_FUNC(_WinGetSockOpt)
#define TSOCKET_NATIVE_GETSOCKNAME NETWORK_FUNC(_WinGetSockName)
#define TSOCKET_NATIVE_GETPEERNAME NETWORK_FUNC(_WinGetPeerName)
#define TSOCKET_NATIVE_SELECT NETWORK_FUNC(_WinSelect)
#define TSOCKET_NATIVE_POLL NETWORK_FUNC(_WinPoll)

#elif OS_POSIX
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
typedef int SOCKET_TYPE(Handle);
typedef socklen_t SOCKET_TYPE(Len);
#define TSOCKET_NATIVE_INVALID (-1)
#define TSOCKET_NATIVE_ERROR (-1)
#define TSOCKET_OS_CLOSE(h) close(h)
#define TSOCKET_NATIVE_ERROR_CODE() errno
#define TSOCKET_NATIVE_HTONS(value) htons(value)
#define TSOCKET_NATIVE_NTOHS(value) ntohs(value)
#define TSOCKET_NATIVE_SOCKET socket
#define TSOCKET_NATIVE_BIND bind
#define TSOCKET_NATIVE_LISTEN listen
#define TSOCKET_NATIVE_ACCEPT accept
#define TSOCKET_NATIVE_CONNECT connect
#define TSOCKET_NATIVE_SEND send
#define TSOCKET_NATIVE_RECV recv
#define TSOCKET_NATIVE_SENDTO sendto
#define TSOCKET_NATIVE_RECVFROM recvfrom
#define TSOCKET_NATIVE_SHUTDOWN shutdown
#define TSOCKET_NATIVE_SETSOCKOPT setsockopt
#define TSOCKET_NATIVE_GETSOCKOPT getsockopt
#define TSOCKET_NATIVE_GETSOCKNAME getsockname
#define TSOCKET_NATIVE_GETPEERNAME getpeername
#define TSOCKET_NATIVE_SELECT select

#endif

static inline bool SOCKET_FUNC(_DescriptorSetContains)(
    SOCKET_TYPE(Handle) handle, const fd_set *set) {
#if OS_WINDOWS
  u_int index;
  if (set == NULL)
    return false;
  for (index = 0U; index < set->fd_count; ++index) {
    if (set->fd_array[index] == handle)
      return true;
  }
  return false;
#else
  return set != NULL && FD_ISSET(handle, set) != 0;
#endif
}

static inline int SOCKET_FUNC(_NativeFamily)(TNetwork_Address_Type type) {
  return type == NETWORK_ADDRESS_TYPE(IPV6) ? AF_INET6 : AF_INET;
}

static inline bool SOCKET_FUNC(_EndpointToNative)(
    const TNetwork_Endpoint *endpoint, struct sockaddr_storage *storage,
    SOCKET_TYPE(Len) *length) {
  memset(storage, 0, sizeof(*storage));
  if (endpoint->address.type == NETWORK_ADDRESS_TYPE(IPV4)) {
    struct sockaddr_in *address = (struct sockaddr_in *)storage;
    address->sin_family = AF_INET;
    address->sin_port = TSOCKET_NATIVE_HTONS(endpoint->port);
    memcpy(&address->sin_addr, endpoint->address.bytes.ipv4, 4);
    *length = (SOCKET_TYPE(Len))sizeof(*address);
    return true;
  }
  if (endpoint->address.type == NETWORK_ADDRESS_TYPE(IPV6)) {
    struct sockaddr_in6 *address = (struct sockaddr_in6 *)storage;
    address->sin6_family = AF_INET6;
    address->sin6_port = TSOCKET_NATIVE_HTONS(endpoint->port);
    memcpy(&address->sin6_addr, endpoint->address.bytes.ipv6, 16);
    *length = (SOCKET_TYPE(Len))sizeof(*address);
    return true;
  }
  return false;
}

static inline bool SOCKET_FUNC(_EndpointFromNative)(
    const struct sockaddr *native, TNetwork_Endpoint *endpoint) {
  if (native->sa_family == AF_INET) {
    const struct sockaddr_in *address = (const struct sockaddr_in *)native;
    endpoint->address.type = NETWORK_ADDRESS_TYPE(IPV4);
    memcpy(endpoint->address.bytes.ipv4, &address->sin_addr, 4);
    endpoint->port = TSOCKET_NATIVE_NTOHS(address->sin_port);
    return true;
  }
  if (native->sa_family == AF_INET6) {
    const struct sockaddr_in6 *address = (const struct sockaddr_in6 *)native;
    endpoint->address.type = NETWORK_ADDRESS_TYPE(IPV6);
    memcpy(endpoint->address.bytes.ipv6, &address->sin6_addr, 16);
    endpoint->port = TSOCKET_NATIVE_NTOHS(address->sin6_port);
    return true;
  }
  return false;
}

static inline TNetwork_Error SOCKET_FUNC(_MapNativeError)(int error) {
#if OS_WINDOWS
  switch (error) {
  case WSAEWOULDBLOCK:
  case WSAEINPROGRESS:
  case WSAEALREADY:
    return NETWORK_ERROR_CONST(WOULD_BLOCK);
  case WSAETIMEDOUT: return NETWORK_ERROR_CONST(TIMEOUT);
  case WSAECONNREFUSED: return NETWORK_ERROR_CONST(CONNECTION_REFUSED);
  case WSAECONNRESET: return NETWORK_ERROR_CONST(CONNECTION_RESET);
  case WSAECONNABORTED: return NETWORK_ERROR_CONST(CONNECTION_ABORTED);
  case WSAENOTCONN: return NETWORK_ERROR_CONST(NOT_CONNECTED);
  case WSAEISCONN: return NETWORK_ERROR_CONST(ALREADY_CONNECTED);
  case WSAEHOSTUNREACH: return NETWORK_ERROR_CONST(HOST_UNREACHABLE);
  case WSAENETUNREACH: return NETWORK_ERROR_CONST(NETWORK_UNREACHABLE);
  case WSAEADDRINUSE: return NETWORK_ERROR_CONST(ADDRESS_IN_USE);
  case WSAEADDRNOTAVAIL: return NETWORK_ERROR_CONST(ADDRESS_UNAVAILABLE);
  case WSAEACCES: return NETWORK_ERROR_CONST(PERMISSION_DENIED);
  case WSAEMSGSIZE: return NETWORK_ERROR_CONST(MESSAGE_TOO_LARGE);
  case WSAEPROTONOSUPPORT: return NETWORK_ERROR_CONST(PROTOCOL_UNSUPPORTED);
  case WSAEAFNOSUPPORT: return NETWORK_ERROR_CONST(ADDRESS_FAMILY_UNSUPPORTED);
  case WSAENOBUFS: return NETWORK_ERROR_CONST(RESOURCE_EXHAUSTED);
  default: return NETWORK_ERROR_CONST(UNKNOWN);
  }
#else
  switch (error) {
  case EAGAIN:
#if EWOULDBLOCK != EAGAIN
  case EWOULDBLOCK:
#endif
  case EINPROGRESS:
  case EALREADY:
    return NETWORK_ERROR_CONST(WOULD_BLOCK);
  case ETIMEDOUT: return NETWORK_ERROR_CONST(TIMEOUT);
  case ECONNREFUSED: return NETWORK_ERROR_CONST(CONNECTION_REFUSED);
  case ECONNRESET: return NETWORK_ERROR_CONST(CONNECTION_RESET);
  case ECONNABORTED: return NETWORK_ERROR_CONST(CONNECTION_ABORTED);
  case ENOTCONN: return NETWORK_ERROR_CONST(NOT_CONNECTED);
  case EISCONN: return NETWORK_ERROR_CONST(ALREADY_CONNECTED);
  case EHOSTUNREACH: return NETWORK_ERROR_CONST(HOST_UNREACHABLE);
  case ENETUNREACH: return NETWORK_ERROR_CONST(NETWORK_UNREACHABLE);
  case EADDRINUSE: return NETWORK_ERROR_CONST(ADDRESS_IN_USE);
  case EADDRNOTAVAIL: return NETWORK_ERROR_CONST(ADDRESS_UNAVAILABLE);
  case EACCES: return NETWORK_ERROR_CONST(PERMISSION_DENIED);
  case EMSGSIZE: return NETWORK_ERROR_CONST(MESSAGE_TOO_LARGE);
  case EPROTONOSUPPORT: return NETWORK_ERROR_CONST(PROTOCOL_UNSUPPORTED);
  case EAFNOSUPPORT: return NETWORK_ERROR_CONST(ADDRESS_FAMILY_UNSUPPORTED);
  case EMFILE:
  case ENFILE:
  case ENOBUFS:
  case ENOMEM:
    return NETWORK_ERROR_CONST(RESOURCE_EXHAUSTED);
  default: return NETWORK_ERROR_CONST(UNKNOWN);
  }
#endif
}

static inline bool SOCKET_FUNC(_SetBlockingNative)(SOCKET_TYPE(Handle) handle,
                                                    bool blocking) {
#if OS_WINDOWS
  u_long mode = blocking ? 0UL : 1UL;
  return NETWORK_FUNC(_WinIoctlSocket)(handle, FIONBIO, &mode) == 0;
#else
  int flags = fcntl(handle, F_GETFL, 0);
  if (flags == -1)
    return false;
  flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
  return fcntl(handle, F_SETFL, flags) == 0;
#endif
}
