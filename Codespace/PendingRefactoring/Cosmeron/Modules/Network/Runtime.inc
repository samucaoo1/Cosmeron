#pragma once

#if OS_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <string.h>

typedef int (WSAAPI *NETWORK_INS(WinSockStartup))(WORD, LPWSADATA);
typedef int (WSAAPI *NETWORK_INS(WinSockGetLastError))(void);
typedef SOCKET (WSAAPI *NETWORK_INS(WinSockSocket))(int, int, int);
typedef int (WSAAPI *NETWORK_INS(WinSockCloseSocket))(SOCKET);
typedef int (WSAAPI *NETWORK_INS(WinSockIoctlSocket))(SOCKET, long, u_long *);
typedef int (WSAAPI *NETWORK_INS(WinSockBind))(SOCKET, const struct sockaddr *, int);
typedef int (WSAAPI *NETWORK_INS(WinSockListen))(SOCKET, int);
typedef SOCKET (WSAAPI *NETWORK_INS(WinSockAccept))(SOCKET, struct sockaddr *, int *);
typedef int (WSAAPI *NETWORK_INS(WinSockConnect))(SOCKET, const struct sockaddr *, int);
typedef int (WSAAPI *NETWORK_INS(WinSockSend))(SOCKET, const char *, int, int);
typedef int (WSAAPI *NETWORK_INS(WinSockReceive))(SOCKET, char *, int, int);
typedef int (WSAAPI *NETWORK_INS(WinSockSendTo))(
    SOCKET, const char *, int, int, const struct sockaddr *, int);
typedef int (WSAAPI *NETWORK_INS(WinSockReceiveFrom))(
    SOCKET, char *, int, int, struct sockaddr *, int *);
typedef int (WSAAPI *NETWORK_INS(WinSockShutdown))(SOCKET, int);
typedef int (WSAAPI *NETWORK_INS(WinSockSetSockOpt))(
    SOCKET, int, int, const char *, int);
typedef int (WSAAPI *NETWORK_INS(WinSockGetSockOpt))(
    SOCKET, int, int, char *, int *);
typedef int (WSAAPI *NETWORK_INS(WinSockGetSockName))(
    SOCKET, struct sockaddr *, int *);
typedef int (WSAAPI *NETWORK_INS(WinSockGetPeerName))(
    SOCKET, struct sockaddr *, int *);
typedef int (WSAAPI *NETWORK_INS(WinSockSelect))(
    int, fd_set *, fd_set *, fd_set *, const struct timeval *);
typedef int (WSAAPI *NETWORK_INS(WinSockGetAddrInfo))(
    PCSTR, PCSTR, const ADDRINFOA *, PADDRINFOA *);
typedef void (WSAAPI *NETWORK_INS(WinSockFreeAddrInfo))(PADDRINFOA);
typedef int (WSAAPI *NETWORK_INS(WinSockPoll))(LPWSAPOLLFD, ULONG, INT);
typedef u_short (WSAAPI *NETWORK_INS(WinSockHostToNetworkShort))(u_short);
typedef u_short (WSAAPI *NETWORK_INS(WinSockNetworkToHostShort))(u_short);

typedef struct NETWORK_INS(WinSockAPI) {
  HMODULE module;
  NETWORK_INS(WinSockStartup) startup;
  NETWORK_INS(WinSockGetLastError) getLastError;
  NETWORK_INS(WinSockSocket) socket;
  NETWORK_INS(WinSockCloseSocket) closeSocket;
  NETWORK_INS(WinSockIoctlSocket) ioctlSocket;
  NETWORK_INS(WinSockBind) bind;
  NETWORK_INS(WinSockListen) listen;
  NETWORK_INS(WinSockAccept) accept;
  NETWORK_INS(WinSockConnect) connect;
  NETWORK_INS(WinSockSend) send;
  NETWORK_INS(WinSockReceive) receive;
  NETWORK_INS(WinSockSendTo) sendTo;
  NETWORK_INS(WinSockReceiveFrom) receiveFrom;
  NETWORK_INS(WinSockShutdown) shutdown;
  NETWORK_INS(WinSockSetSockOpt) setSockOpt;
  NETWORK_INS(WinSockGetSockOpt) getSockOpt;
  NETWORK_INS(WinSockGetSockName) getSockName;
  NETWORK_INS(WinSockGetPeerName) getPeerName;
  NETWORK_INS(WinSockSelect) select;
  NETWORK_INS(WinSockGetAddrInfo) getAddrInfo;
  NETWORK_INS(WinSockFreeAddrInfo) freeAddrInfo;
  NETWORK_INS(WinSockPoll) poll;
  NETWORK_INS(WinSockHostToNetworkShort) hostToNetworkShort;
  NETWORK_INS(WinSockNetworkToHostShort) networkToHostShort;
} NETWORK_INS(WinSockAPI);

static inline NETWORK_INS(WinSockAPI) *NETWORK_INS(WinSockAPIGet)(void) {
  static NETWORK_INS(WinSockAPI) api = {0};
  return &api;
}

static inline bool NETWORK_INS(WinSockAPILoad)(NETWORK_INS(WinSockAPI) *api) {
  if (api == NULL)
    return false;

  api->module = LoadLibraryA("ws2_32.dll");
  if (api->module == NULL)
    return false;

#define COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(MEMBER, SYMBOL)                            \
  do {                                                                          \
    FARPROC symbolAddress = GetProcAddress(api->module, (SYMBOL));              \
    if (symbolAddress == NULL || sizeof(api->MEMBER) != sizeof(symbolAddress))  \
      goto load_failed;                                                         \
    memcpy(&api->MEMBER, &symbolAddress, sizeof(api->MEMBER));                  \
  } while (0)

  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(startup, "WSAStartup");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(getLastError, "WSAGetLastError");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(socket, "socket");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(closeSocket, "closesocket");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(ioctlSocket, "ioctlsocket");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(bind, "bind");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(listen, "listen");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(accept, "accept");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(connect, "connect");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(send, "send");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(receive, "recv");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(sendTo, "sendto");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(receiveFrom, "recvfrom");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(shutdown, "shutdown");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(setSockOpt, "setsockopt");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(getSockOpt, "getsockopt");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(getSockName, "getsockname");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(getPeerName, "getpeername");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(select, "select");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(getAddrInfo, "getaddrinfo");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(freeAddrInfo, "freeaddrinfo");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(poll, "WSAPoll");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(hostToNetworkShort, "htons");
  COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL(networkToHostShort, "ntohs");

#undef COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL
  return true;

load_failed:
#undef COSMERON_MACRO_INTERNAL_NETWORK_LOAD_WINSOCK_SYMBOL
  FreeLibrary(api->module);
  memset(api, 0, sizeof(*api));
  return false;
}

static inline BOOL CALLBACK NETWORK_INS(WinSockInitialize)(
    PINIT_ONCE initOnce, PVOID parameter, PVOID *context) {
  bool *initialized = (bool *)parameter;
  NETWORK_INS(WinSockAPI) *api = NETWORK_INS(WinSockAPIGet)();
  WSADATA data;
  (void)initOnce;
  (void)context;

  *initialized = NETWORK_INS(WinSockAPILoad)(api) &&
                 api->startup(MAKEWORD(2, 2), &data) == 0;
  if (!*initialized && api->module != NULL) {
    FreeLibrary(api->module);
    memset(api, 0, sizeof(*api));
  }
  return TRUE;
}

static inline bool NETWORK_INS(EnsureRuntime)(void) {
  static INIT_ONCE once = INIT_ONCE_STATIC_INIT;
  static bool initialized = false;
  if (!InitOnceExecuteOnce(&once, NETWORK_INS(WinSockInitialize),
                           &initialized, NULL))
    return false;
  return initialized;
}

static inline SOCKET NETWORK_INS(WinSocket)(int family, int kind, int protocol) {
  return NETWORK_INS(WinSockAPIGet)()->socket(family, kind, protocol);
}
static inline int NETWORK_INS(WinCloseSocket)(SOCKET socket) {
  return NETWORK_INS(WinSockAPIGet)()->closeSocket(socket);
}
static inline int NETWORK_INS(WinIoctlSocket)(SOCKET socket, long command,
                                                u_long *argument) {
  return NETWORK_INS(WinSockAPIGet)()->ioctlSocket(socket, command, argument);
}
static inline int NETWORK_INS(WinGetLastError)(void) {
  return NETWORK_INS(WinSockAPIGet)()->getLastError();
}
static inline int NETWORK_INS(WinBind)(
    SOCKET socket, const struct sockaddr *address, int length) {
  return NETWORK_INS(WinSockAPIGet)()->bind(socket, address, length);
}
static inline int NETWORK_INS(WinListen)(SOCKET socket, int backlog) {
  return NETWORK_INS(WinSockAPIGet)()->listen(socket, backlog);
}
static inline SOCKET NETWORK_INS(WinAccept)(
    SOCKET socket, struct sockaddr *address, int *length) {
  return NETWORK_INS(WinSockAPIGet)()->accept(socket, address, length);
}
static inline int NETWORK_INS(WinConnect)(
    SOCKET socket, const struct sockaddr *address, int length) {
  return NETWORK_INS(WinSockAPIGet)()->connect(socket, address, length);
}
static inline int NETWORK_INS(WinSend)(
    SOCKET socket, const char *buffer, int length, int flags) {
  return NETWORK_INS(WinSockAPIGet)()->send(socket, buffer, length, flags);
}
static inline int NETWORK_INS(WinReceive)(
    SOCKET socket, char *buffer, int length, int flags) {
  return NETWORK_INS(WinSockAPIGet)()->receive(socket, buffer, length, flags);
}
static inline int NETWORK_INS(WinSendTo)(
    SOCKET socket, const char *buffer, int length, int flags,
    const struct sockaddr *address, int addressLength) {
  return NETWORK_INS(WinSockAPIGet)()->sendTo(
      socket, buffer, length, flags, address, addressLength);
}
static inline int NETWORK_INS(WinReceiveFrom)(
    SOCKET socket, char *buffer, int length, int flags,
    struct sockaddr *address, int *addressLength) {
  return NETWORK_INS(WinSockAPIGet)()->receiveFrom(
      socket, buffer, length, flags, address, addressLength);
}
static inline int NETWORK_INS(WinShutdown)(SOCKET socket, int direction) {
  return NETWORK_INS(WinSockAPIGet)()->shutdown(socket, direction);
}
static inline int NETWORK_INS(WinSetSockOpt)(
    SOCKET socket, int level, int option, const char *value, int length) {
  return NETWORK_INS(WinSockAPIGet)()->setSockOpt(
      socket, level, option, value, length);
}
static inline int NETWORK_INS(WinGetSockOpt)(
    SOCKET socket, int level, int option, char *value, int *length) {
  return NETWORK_INS(WinSockAPIGet)()->getSockOpt(
      socket, level, option, value, length);
}
static inline int NETWORK_INS(WinGetSockName)(
    SOCKET socket, struct sockaddr *address, int *length) {
  return NETWORK_INS(WinSockAPIGet)()->getSockName(socket, address, length);
}
static inline int NETWORK_INS(WinGetPeerName)(
    SOCKET socket, struct sockaddr *address, int *length) {
  return NETWORK_INS(WinSockAPIGet)()->getPeerName(socket, address, length);
}
static inline int NETWORK_INS(WinSelect)(
    int count, fd_set *readSet, fd_set *writeSet, fd_set *errorSet,
    const struct timeval *timeout) {
  return NETWORK_INS(WinSockAPIGet)()->select(
      count, readSet, writeSet, errorSet, timeout);
}
static inline int NETWORK_INS(WinGetAddrInfo)(
    PCSTR host, PCSTR service, const ADDRINFOA *hints, PADDRINFOA *result) {
  return NETWORK_INS(WinSockAPIGet)()->getAddrInfo(host, service, hints, result);
}
static inline void NETWORK_INS(WinFreeAddrInfo)(PADDRINFOA result) {
  NETWORK_INS(WinSockAPIGet)()->freeAddrInfo(result);
}
static inline int NETWORK_INS(WinPoll)(
    LPWSAPOLLFD descriptors, ULONG count, INT timeout) {
  return NETWORK_INS(WinSockAPIGet)()->poll(descriptors, count, timeout);
}
static inline u_short NETWORK_INS(WinHostToNetworkShort)(u_short value) {
  return NETWORK_INS(WinSockAPIGet)()->hostToNetworkShort(value);
}
static inline u_short NETWORK_INS(WinNetworkToHostShort)(u_short value) {
  return NETWORK_INS(WinSockAPIGet)()->networkToHostShort(value);
}
#else
static inline bool NETWORK_INS(EnsureRuntime)(void) { return true; }
#endif
