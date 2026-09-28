#pragma once

#if OS_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <string.h>

typedef int (WSAAPI *TNetwork_WinSock_Startup)(WORD, LPWSADATA);
typedef int (WSAAPI *TNetwork_WinSock_GetLastError)(void);
typedef SOCKET (WSAAPI *TNetwork_WinSock_Socket)(int, int, int);
typedef int (WSAAPI *TNetwork_WinSock_CloseSocket)(SOCKET);
typedef int (WSAAPI *TNetwork_WinSock_IoctlSocket)(SOCKET, long, u_long *);
typedef int (WSAAPI *TNetwork_WinSock_Bind)(SOCKET, const struct sockaddr *, int);
typedef int (WSAAPI *TNetwork_WinSock_Listen)(SOCKET, int);
typedef SOCKET (WSAAPI *TNetwork_WinSock_Accept)(SOCKET, struct sockaddr *, int *);
typedef int (WSAAPI *TNetwork_WinSock_Connect)(SOCKET, const struct sockaddr *, int);
typedef int (WSAAPI *TNetwork_WinSock_Send)(SOCKET, const char *, int, int);
typedef int (WSAAPI *TNetwork_WinSock_Receive)(SOCKET, char *, int, int);
typedef int (WSAAPI *TNetwork_WinSock_SendTo)(
    SOCKET, const char *, int, int, const struct sockaddr *, int);
typedef int (WSAAPI *TNetwork_WinSock_ReceiveFrom)(
    SOCKET, char *, int, int, struct sockaddr *, int *);
typedef int (WSAAPI *TNetwork_WinSock_Shutdown)(SOCKET, int);
typedef int (WSAAPI *TNetwork_WinSock_SetSockOpt)(
    SOCKET, int, int, const char *, int);
typedef int (WSAAPI *TNetwork_WinSock_GetSockOpt)(
    SOCKET, int, int, char *, int *);
typedef int (WSAAPI *TNetwork_WinSock_GetSockName)(
    SOCKET, struct sockaddr *, int *);
typedef int (WSAAPI *TNetwork_WinSock_GetPeerName)(
    SOCKET, struct sockaddr *, int *);
typedef int (WSAAPI *TNetwork_WinSock_Select)(
    int, fd_set *, fd_set *, fd_set *, const struct timeval *);
typedef int (WSAAPI *TNetwork_WinSock_GetAddrInfo)(
    PCSTR, PCSTR, const ADDRINFOA *, PADDRINFOA *);
typedef void (WSAAPI *TNetwork_WinSock_FreeAddrInfo)(PADDRINFOA);
typedef int (WSAAPI *TNetwork_WinSock_Poll)(LPWSAPOLLFD, ULONG, INT);
typedef u_short (WSAAPI *TNetwork_WinSock_HostToNetworkShort)(u_short);
typedef u_short (WSAAPI *TNetwork_WinSock_NetworkToHostShort)(u_short);

typedef struct TNetwork_WinSock_API {
  HMODULE module;
  TNetwork_WinSock_Startup startup;
  TNetwork_WinSock_GetLastError getLastError;
  TNetwork_WinSock_Socket socket;
  TNetwork_WinSock_CloseSocket closeSocket;
  TNetwork_WinSock_IoctlSocket ioctlSocket;
  TNetwork_WinSock_Bind bind;
  TNetwork_WinSock_Listen listen;
  TNetwork_WinSock_Accept accept;
  TNetwork_WinSock_Connect connect;
  TNetwork_WinSock_Send send;
  TNetwork_WinSock_Receive receive;
  TNetwork_WinSock_SendTo sendTo;
  TNetwork_WinSock_ReceiveFrom receiveFrom;
  TNetwork_WinSock_Shutdown shutdown;
  TNetwork_WinSock_SetSockOpt setSockOpt;
  TNetwork_WinSock_GetSockOpt getSockOpt;
  TNetwork_WinSock_GetSockName getSockName;
  TNetwork_WinSock_GetPeerName getPeerName;
  TNetwork_WinSock_Select select;
  TNetwork_WinSock_GetAddrInfo getAddrInfo;
  TNetwork_WinSock_FreeAddrInfo freeAddrInfo;
  TNetwork_WinSock_Poll poll;
  TNetwork_WinSock_HostToNetworkShort hostToNetworkShort;
  TNetwork_WinSock_NetworkToHostShort networkToHostShort;
} TNetwork_WinSock_API;

static inline TNetwork_WinSock_API *NETWORK_FUNC(_WinSockAPI)(void) {
  static TNetwork_WinSock_API api = {0};
  return &api;
}

static inline bool NETWORK_FUNC(_LoadWinSockAPI)(TNetwork_WinSock_API *api) {
  if (api == NULL)
    return false;

  api->module = LoadLibraryA("ws2_32.dll");
  if (api->module == NULL)
    return false;

#define NETWORK_LOAD_WINSOCK_SYMBOL(MEMBER, SYMBOL)                            \
  do {                                                                          \
    FARPROC symbolAddress = GetProcAddress(api->module, (SYMBOL));              \
    if (symbolAddress == NULL || sizeof(api->MEMBER) != sizeof(symbolAddress))  \
      goto load_failed;                                                         \
    memcpy(&api->MEMBER, &symbolAddress, sizeof(api->MEMBER));                  \
  } while (0)

  NETWORK_LOAD_WINSOCK_SYMBOL(startup, "WSAStartup");
  NETWORK_LOAD_WINSOCK_SYMBOL(getLastError, "WSAGetLastError");
  NETWORK_LOAD_WINSOCK_SYMBOL(socket, "socket");
  NETWORK_LOAD_WINSOCK_SYMBOL(closeSocket, "closesocket");
  NETWORK_LOAD_WINSOCK_SYMBOL(ioctlSocket, "ioctlsocket");
  NETWORK_LOAD_WINSOCK_SYMBOL(bind, "bind");
  NETWORK_LOAD_WINSOCK_SYMBOL(listen, "listen");
  NETWORK_LOAD_WINSOCK_SYMBOL(accept, "accept");
  NETWORK_LOAD_WINSOCK_SYMBOL(connect, "connect");
  NETWORK_LOAD_WINSOCK_SYMBOL(send, "send");
  NETWORK_LOAD_WINSOCK_SYMBOL(receive, "recv");
  NETWORK_LOAD_WINSOCK_SYMBOL(sendTo, "sendto");
  NETWORK_LOAD_WINSOCK_SYMBOL(receiveFrom, "recvfrom");
  NETWORK_LOAD_WINSOCK_SYMBOL(shutdown, "shutdown");
  NETWORK_LOAD_WINSOCK_SYMBOL(setSockOpt, "setsockopt");
  NETWORK_LOAD_WINSOCK_SYMBOL(getSockOpt, "getsockopt");
  NETWORK_LOAD_WINSOCK_SYMBOL(getSockName, "getsockname");
  NETWORK_LOAD_WINSOCK_SYMBOL(getPeerName, "getpeername");
  NETWORK_LOAD_WINSOCK_SYMBOL(select, "select");
  NETWORK_LOAD_WINSOCK_SYMBOL(getAddrInfo, "getaddrinfo");
  NETWORK_LOAD_WINSOCK_SYMBOL(freeAddrInfo, "freeaddrinfo");
  NETWORK_LOAD_WINSOCK_SYMBOL(poll, "WSAPoll");
  NETWORK_LOAD_WINSOCK_SYMBOL(hostToNetworkShort, "htons");
  NETWORK_LOAD_WINSOCK_SYMBOL(networkToHostShort, "ntohs");

#undef NETWORK_LOAD_WINSOCK_SYMBOL
  return true;

load_failed:
#undef NETWORK_LOAD_WINSOCK_SYMBOL
  FreeLibrary(api->module);
  memset(api, 0, sizeof(*api));
  return false;
}

static inline BOOL CALLBACK NETWORK_FUNC(_InitializeWinSock)(
    PINIT_ONCE initOnce, PVOID parameter, PVOID *context) {
  bool *initialized = (bool *)parameter;
  TNetwork_WinSock_API *api = NETWORK_FUNC(_WinSockAPI)();
  WSADATA data;
  (void)initOnce;
  (void)context;

  *initialized = NETWORK_FUNC(_LoadWinSockAPI)(api) &&
                 api->startup(MAKEWORD(2, 2), &data) == 0;
  if (!*initialized && api->module != NULL) {
    FreeLibrary(api->module);
    memset(api, 0, sizeof(*api));
  }
  return TRUE;
}

static inline bool NETWORK_FUNC(_EnsureRuntime)(void) {
  static INIT_ONCE once = INIT_ONCE_STATIC_INIT;
  static bool initialized = false;
  if (!InitOnceExecuteOnce(&once, NETWORK_FUNC(_InitializeWinSock),
                           &initialized, NULL))
    return false;
  return initialized;
}

static inline SOCKET NETWORK_FUNC(_WinSocket)(int family, int kind, int protocol) {
  return NETWORK_FUNC(_WinSockAPI)()->socket(family, kind, protocol);
}
static inline int NETWORK_FUNC(_WinCloseSocket)(SOCKET socket) {
  return NETWORK_FUNC(_WinSockAPI)()->closeSocket(socket);
}
static inline int NETWORK_FUNC(_WinIoctlSocket)(SOCKET socket, long command,
                                                u_long *argument) {
  return NETWORK_FUNC(_WinSockAPI)()->ioctlSocket(socket, command, argument);
}
static inline int NETWORK_FUNC(_WinGetLastError)(void) {
  return NETWORK_FUNC(_WinSockAPI)()->getLastError();
}
static inline int NETWORK_FUNC(_WinBind)(
    SOCKET socket, const struct sockaddr *address, int length) {
  return NETWORK_FUNC(_WinSockAPI)()->bind(socket, address, length);
}
static inline int NETWORK_FUNC(_WinListen)(SOCKET socket, int backlog) {
  return NETWORK_FUNC(_WinSockAPI)()->listen(socket, backlog);
}
static inline SOCKET NETWORK_FUNC(_WinAccept)(
    SOCKET socket, struct sockaddr *address, int *length) {
  return NETWORK_FUNC(_WinSockAPI)()->accept(socket, address, length);
}
static inline int NETWORK_FUNC(_WinConnect)(
    SOCKET socket, const struct sockaddr *address, int length) {
  return NETWORK_FUNC(_WinSockAPI)()->connect(socket, address, length);
}
static inline int NETWORK_FUNC(_WinSend)(
    SOCKET socket, const char *buffer, int length, int flags) {
  return NETWORK_FUNC(_WinSockAPI)()->send(socket, buffer, length, flags);
}
static inline int NETWORK_FUNC(_WinReceive)(
    SOCKET socket, char *buffer, int length, int flags) {
  return NETWORK_FUNC(_WinSockAPI)()->receive(socket, buffer, length, flags);
}
static inline int NETWORK_FUNC(_WinSendTo)(
    SOCKET socket, const char *buffer, int length, int flags,
    const struct sockaddr *address, int addressLength) {
  return NETWORK_FUNC(_WinSockAPI)()->sendTo(
      socket, buffer, length, flags, address, addressLength);
}
static inline int NETWORK_FUNC(_WinReceiveFrom)(
    SOCKET socket, char *buffer, int length, int flags,
    struct sockaddr *address, int *addressLength) {
  return NETWORK_FUNC(_WinSockAPI)()->receiveFrom(
      socket, buffer, length, flags, address, addressLength);
}
static inline int NETWORK_FUNC(_WinShutdown)(SOCKET socket, int direction) {
  return NETWORK_FUNC(_WinSockAPI)()->shutdown(socket, direction);
}
static inline int NETWORK_FUNC(_WinSetSockOpt)(
    SOCKET socket, int level, int option, const char *value, int length) {
  return NETWORK_FUNC(_WinSockAPI)()->setSockOpt(
      socket, level, option, value, length);
}
static inline int NETWORK_FUNC(_WinGetSockOpt)(
    SOCKET socket, int level, int option, char *value, int *length) {
  return NETWORK_FUNC(_WinSockAPI)()->getSockOpt(
      socket, level, option, value, length);
}
static inline int NETWORK_FUNC(_WinGetSockName)(
    SOCKET socket, struct sockaddr *address, int *length) {
  return NETWORK_FUNC(_WinSockAPI)()->getSockName(socket, address, length);
}
static inline int NETWORK_FUNC(_WinGetPeerName)(
    SOCKET socket, struct sockaddr *address, int *length) {
  return NETWORK_FUNC(_WinSockAPI)()->getPeerName(socket, address, length);
}
static inline int NETWORK_FUNC(_WinSelect)(
    int count, fd_set *readSet, fd_set *writeSet, fd_set *errorSet,
    const struct timeval *timeout) {
  return NETWORK_FUNC(_WinSockAPI)()->select(
      count, readSet, writeSet, errorSet, timeout);
}
static inline int NETWORK_FUNC(_WinGetAddrInfo)(
    PCSTR host, PCSTR service, const ADDRINFOA *hints, PADDRINFOA *result) {
  return NETWORK_FUNC(_WinSockAPI)()->getAddrInfo(host, service, hints, result);
}
static inline void NETWORK_FUNC(_WinFreeAddrInfo)(PADDRINFOA result) {
  NETWORK_FUNC(_WinSockAPI)()->freeAddrInfo(result);
}
static inline int NETWORK_FUNC(_WinPoll)(
    LPWSAPOLLFD descriptors, ULONG count, INT timeout) {
  return NETWORK_FUNC(_WinSockAPI)()->poll(descriptors, count, timeout);
}
static inline u_short NETWORK_FUNC(_WinHostToNetworkShort)(u_short value) {
  return NETWORK_FUNC(_WinSockAPI)()->hostToNetworkShort(value);
}
static inline u_short NETWORK_FUNC(_WinNetworkToHostShort)(u_short value) {
  return NETWORK_FUNC(_WinSockAPI)()->networkToHostShort(value);
}
#else
static inline bool NETWORK_FUNC(_EnsureRuntime)(void) { return true; }
#endif
