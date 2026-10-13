# Network

The **Network** module provides portable IPv4/IPv6 addressing, host resolution, native socket control, higher-level TCP connections and listeners, UDP datagrams, and synchronous readiness events. It follows the library's C11 header-included API pattern, without exposing native socket family constants as the normal public interface.

There is **no single public `Network.h` umbrella header** in this branch. Include the header for the package you need; dependencies pull in lower-level declarations.

```c
#include "Cosmeron/Modules/Network/Connection/Connection.h"
/* Or: Address/Address.h, Resolve/Resolve.h, Socket/Socket.h,
 *     Datagram/Datagram.h, Listener/Listener.h, Poll/Poll.h */
```

---

# Overview

| Package | Header | Purpose | Public operations |
| --- | --- | --- | ---: |
| [Address](#address-package) | `Address/Address.h` | IPv4/IPv6 binary addresses, ports and endpoints. | 7 |
| [Resolve](#resolve-package) | `Resolve/Resolve.h` | Hostname resolution into a caller-owned vector of unique IP addresses. | 1 |
| [Socket](#socket-package) | `Socket/Socket.h` | Low-level platform-independent TCP/UDP socket lifecycle, I/O and options. | 16 |
| [Connection](#connection-package) | `Connection/Connection.h` | Hostname-oriented TCP connections, timeouts, reconnection and reliable block I/O. | 15 |
| [Datagram](#datagram-package) | `Datagram/Datagram.h` | UDP endpoint management, addressed datagrams and optional connected peers. | 14 |
| [Listener](#listener-package) | `Listener/Listener.h` | TCP listening with IPv4/IPv6 fallback and accepted connections. | 6 |
| [Poller](#poller-package) | `Poll/Poll.h` | Readiness events for connections, listeners and datagram sockets. | 14 |
| Error | `Error/Error.h` | Network-specific error enum used by the packages | No functions |

### Macro form

```c
NETWORK_ADDRESS_TYPE(TAddress) address;
NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&address);
```

### Direct form

```c
Network_Address_TAddress address;
Network_Address_IPv4_Loopback(&address);
```

Both names address the same function in the default namespace. When `COSMERON_NAMESPACE` is configured before inclusion, the direct C names change; use macro forms for namespace-independent code.

### Abstraction layers

| Layer | Packages | Typical usage |
| --- | --- | --- |
| Value and lookup | Address, Resolve | Endpoint construction and host resolution |
| Native control | Socket | Explicit family/kind/protocol and socket options |
| Higher-level TCP/UDP | Connection, Listener, Datagram | Most client/server applications |
| Readiness events | Poller | Event-based multiple-socket I/O |
| Error domain | Error | OS/network failure classification separate from OPSTATUS |

---

# General contracts

- **Status:** fallible functions return `OPSTATUS` and may write a more specific `NETWORK_TYPE(TError)` to an optional `error` output. The two results are distinct: for example, a native WOULD_BLOCK normally returns `STATUS_CONST(GENERIC_ERROR)` and sets `NETWORK_ERROR_CONST(WOULD_BLOCK)`. Check both.
- **Native sockets:** always initialize socket-owning objects before use. Some invalid-handle values differ from zero between platforms, notably on POSIX. `SOCKET_FUNC(Init)` establishes the correct sentinel.
- **Data sizes:** send/receive lengths and counters are **bytes**, not element counts. TCP sends/reads may be short and must not be assumed to preserve message boundaries.
- **Ownership:** Socket, Connection, Datagram and Listener own native handles. Connection also owns a hostname copy; Resolve and Poller own Container vectors. Events/registrations borrow their network objects; the poller never destroys them.
- **Threading:** the module does not automatically serialize socket operations. Avoid moving or destroying objects while a poller is using their addresses.
- **Portability:** backend code maps Cosmeron enums to the OS socket API. Native readiness and error behavior, IPv6 availability, dual-stack support, zero-length UDP behavior and event flags may differ among platforms.

---

# Types and constants

## Error domain

`NETWORK_TYPE(TError)` (direct: `Network_TError`) provides these constants through `NETWORK_ERROR_CONST(NAME)`:

| Error | Meaning |
| --- | --- |
| `NONE` | No network-specific error |
| `TIMEOUT`, `WOULD_BLOCK` | Timeout or nonblocking operation not ready |
| `CONNECTION_REFUSED`, `CONNECTION_RESET`, `CONNECTION_CLOSED`, `CONNECTION_ABORTED` | TCP/connection outcomes |
| `NOT_CONNECTED`, `ALREADY_CONNECTED` | Connection state errors |
| `RESOLVE_HOST_NOT_FOUND` | Hostname resolution failure |
| `HOST_UNREACHABLE`, `NETWORK_UNREACHABLE` | Routing/reachability failure |
| `ADDRESS_INVALID`, `ADDRESS_IN_USE`, `ADDRESS_UNAVAILABLE` | Invalid or unavailable binding/endpoint |
| `PERMISSION_DENIED`, `RESOURCE_EXHAUSTED` | Permission or resource problems |
| `PROTOCOL_UNSUPPORTED`, `ADDRESS_FAMILY_UNSUPPORTED` | Unsupported family/protocol |
| `MESSAGE_TOO_LARGE`, `UNKNOWN` | Datagram size or unclassified native error |

These are the known enum members, not a guarantee that every platform maps every error. Resolve maps common `getaddrinfo` codes, and the Socket backend maps POSIX `errno`/Windows WSA codes. An `INVALID_ARGUMENT` return may deliberately set the domain error to `NONE`.

## Poller events

| Flag | Bit | Meaning |
| --- | ---: | --- |
| `NETWORK_EVENT_CONST(NONE)` | 0 | No requested readiness |
| `NETWORK_EVENT_CONST(READ)` | 0 | Data may be readable |
| `NETWORK_EVENT_CONST(WRITE)` | 1 | Sending may proceed |
| `NETWORK_EVENT_CONST(ACCEPT)` | 2 | Listener may accept |
| `NETWORK_EVENT_CONST(CLOSE)` | 3 | Stream close/hangup indication |
| `NETWORK_EVENT_CONST(ERROR)` | 4 | Native readiness/error indication |

Use bitwise OR to combine interests. Availability of specific flags depends on the OS backend and registered source. Use `NETWORK_POLLER_FUNC(Event_Has)` to inspect events.

---

# API reference

## Function summary

| Package | Function | Description |
| --- | --- | --- |
| Address | [`IPv4_Create`](#address-ipv4_create) | Builds an IPv4 address from four octets. |
| Address | [`IPv4_Any`](#address-ipv4_any) | Builds the IPv4 wildcard address 0.0.0.0. |
| Address | [`IPv4_Loopback`](#address-ipv4_loopback) | Builds the IPv4 loopback address 127.0.0.1. |
| Address | [`IPv6_Any`](#address-ipv6_any) | Builds the IPv6 wildcard address ::. |
| Address | [`IPv6_Loopback`](#address-ipv6_loopback) | Builds the IPv6 loopback address ::1. |
| Address | [`Endpoint_Create`](#address-endpoint_create) | Combines an address and a 16-bit host-order port. |
| Address | [`Equal`](#address-equal) | Compares two IP address values. |
| Resolve | [`Address`](#resolve-address) | Resolves a hostname into an owning vector of unique IPv4/IPv6 addresses. |
| Socket | [`Init`](#socket-init) | Initializes a socket object with an invalid native handle. |
| Socket | [`Create`](#socket-create) | Creates a native socket with the requested family, kind and protocol. |
| Socket | [`Close`](#socket-close) | Closes a native socket and invalidates its handle. |
| Socket | [`Bind`](#socket-bind) | Binds a native socket to an IP endpoint. |
| Socket | [`Listen`](#socket-listen) | Places a bound stream socket into listening mode. |
| Socket | [`Accept`](#socket-accept) | Accepts a client connection from a listening socket. |
| Socket | [`Connect`](#socket-connect) | Connects a socket to a remote endpoint. |
| Socket | [`Send`](#socket-send) | Sends up to the specified number of bytes. |
| Socket | [`Receive`](#socket-receive) | Receives bytes from a connected socket. |
| Socket | [`SendTo`](#socket-sendto) | Sends a datagram to a specified endpoint. |
| Socket | [`ReceiveFrom`](#socket-receivefrom) | Receives a datagram and optionally reports the sender endpoint. |
| Socket | [`Shutdown`](#socket-shutdown) | Shuts down one or both directions of a connected socket. |
| Socket | [`SetBlocking`](#socket-setblocking) | Selects blocking or nonblocking socket operation. |
| Socket | [`SetReuseAddress`](#socket-setreuseaddress) | Enables or disables SO_REUSEADDR. |
| Socket | [`SetKeepAlive`](#socket-setkeepalive) | Enables or disables SO_KEEPALIVE. |
| Socket | [`SetNoDelay`](#socket-setnodelay) | Enables or disables TCP_NODELAY. |
| Connection | [`Init`](#connection-init) | Initializes a high-level TCP connection object. |
| Connection | [`Connect`](#connection-connect) | Connects to a hostname and port using the default timeout. |
| Connection | [`ConnectFor`](#connection-connectfor) | Connects with an explicit overall timeout. |
| Connection | [`Reconnect`](#connection-reconnect) | Reconnects using the stored hostname, port and timeout. |
| Connection | [`Read`](#connection-read) | Reads up to a given byte capacity from an established TCP connection. |
| Connection | [`ReadExact`](#connection-readexact) | Reads exactly `size` bytes, looping until complete or an error. |
| Connection | [`Write`](#connection-write) | Writes up to `size` bytes using one TCP send. |
| Connection | [`WriteAll`](#connection-writeall) | Writes exactly `size` bytes by looping over Write. |
| Connection | [`Shutdown`](#connection-shutdown) | Shuts down TCP read, write, or both directions. |
| Connection | [`SetBlocking`](#connection-setblocking) | Configures whether future I/O calls use blocking mode. |
| Connection | [`Close`](#connection-close) | Closes the underlying TCP socket but retains reconnect metadata. |
| Connection | [`Destroy`](#connection-destroy) | Closes a connection and releases its owned hostname. |
| Connection | [`State`](#connection-state) | Reads the current high-level connection state. |
| Connection | [`LocalEndpoint`](#connection-localendpoint) | Returns the active TCP connection's local endpoint. |
| Connection | [`RemoteEndpoint`](#connection-remoteendpoint) | Returns the connected remote endpoint. |
| Datagram | [`Init`](#datagram-init) | Initializes the UDP object with no open socket. |
| Datagram | [`Open`](#datagram-open) | Opens an IPv4 or IPv6 UDP socket. |
| Datagram | [`Bind`](#datagram-bind) | Binds a UDP socket to a local endpoint. |
| Datagram | [`SetPeer`](#datagram-setpeer) | Associates a UDP peer through a native datagram connect. |
| Datagram | [`SendTo`](#datagram-sendto) | Sends a datagram to an explicitly named endpoint. |
| Datagram | [`ReceiveFrom`](#datagram-receivefrom) | Receives a datagram with optional source endpoint. |
| Datagram | [`Send`](#datagram-send) | Sends through the associated UDP peer. |
| Datagram | [`Receive`](#datagram-receive) | Reads datagrams from the associated UDP peer. |
| Datagram | [`SetBlocking`](#datagram-setblocking) | Sets the UDP socket's native blocking mode. |
| Datagram | [`ClearPeer`](#datagram-clearpeer) | Clears the previously configured UDP peer association. |
| Datagram | [`Close`](#datagram-close) | Closes UDP socket and forgets that a peer was configured. |
| Datagram | [`HasPeer`](#datagram-haspeer) | Tests whether a UDP peer is configured. |
| Datagram | [`LocalEndpoint`](#datagram-localendpoint) | Reports the cached local UDP endpoint. |
| Datagram | [`PeerEndpoint`](#datagram-peerendpoint) | Reports the configured UDP peer endpoint. |
| Listener | [`Init`](#listener-init) | Initializes a listening TCP server object. |
| Listener | [`Listen`](#listener-listen) | Listens on a port, preferring IPv6 with IPv4 fallback. |
| Listener | [`ListenAt`](#listener-listenat) | Listens on an explicit local IP endpoint. |
| Listener | [`Accept`](#listener-accept) | Accepts an incoming TCP client as a high-level connection. |
| Listener | [`Close`](#listener-close) | Closes the listener's primary and secondary sockets. |
| Listener | [`LocalEndpoint`](#listener-localendpoint) | Returns the primary listener's bound endpoint. |
| Poller | [`Init`](#poller-init) | Initializes an empty poller and registration vector. |
| Poller | [`Destroy`](#poller-destroy) | Releases poller registration storage. |
| Poller | [`AddConnection`](#poller-addconnection) | Registers a connection for selected readiness interests. |
| Poller | [`AddListener`](#poller-addlistener) | Registers a listener for readiness/accept events. |
| Poller | [`AddDatagram`](#poller-adddatagram) | Registers a UDP datagram object. |
| Poller | [`Remove`](#poller-remove) | Unregisters a previously returned registration handle. |
| Poller | [`Watch`](#poller-watch) | Changes interests for an existing handle. |
| Poller | [`Wait`](#poller-wait) | Waits for readiness with no time limit. |
| Poller | [`WaitFor`](#poller-waitfor) | Waits for readiness for at most the requested duration. |
| Poller | [`Event_Has`](#poller-event_has) | Tests whether an event contains all requested flag bits. |
| Poller | [`Event_UserData`](#poller-event_userdata) | Retrieves the user pointer captured at registration time. |
| Poller | [`Event_Connection`](#poller-event_connection) | Retrieves the connection associated with a connection event. |
| Poller | [`Event_Listener`](#poller-event_listener) | Retrieves the listener associated with a listener event. |
| Poller | [`Event_Datagram`](#poller-event_datagram) | Retrieves the datagram object associated with a UDP event. |

Each function section shows its **actual declared prototype**, rendered once through the macro naming system and once as a default-namespace C symbol. Usage examples are focused snippets; where identifiers are not declared locally, assume a correctly initialized object of the type shown in the parameters table.

---

# Address package

Header: `Cosmeron/Modules/Network/Address/Address.h`

The `TAddress` contains a family tag plus 4/16 raw IP bytes. `TEndpoint` adds a `uint16_t port` in host byte order. Address helpers operate only on values; no DNS, parsing, or formatting is performed here.

### Types

| Type / constant | Contents |
| --- | --- |
| `NETWORK_ADDRESS_TYPE(TType)` | `NETWORK_ADDRESS_CONST(IPV4)`, `NETWORK_ADDRESS_CONST(IPV6)` |
| `NETWORK_ADDRESS_TYPE(TAddress)` | `.type`, `.bytes.ipv4[4]` or `.bytes.ipv6[16]` |
| `NETWORK_ADDRESS_TYPE(TEndpoint)` | `.address`, `.port` (host-order `uint16_t`) |

### Function summary

| Function | Description |
| --- | --- |
| [`IPv4_Create`](#address-ipv4_create) | Builds an IPv4 address from four octets. |
| [`IPv4_Any`](#address-ipv4_any) | Builds the IPv4 wildcard address 0.0.0.0. |
| [`IPv4_Loopback`](#address-ipv4_loopback) | Builds the IPv4 loopback address 127.0.0.1. |
| [`IPv6_Any`](#address-ipv6_any) | Builds the IPv6 wildcard address ::. |
| [`IPv6_Loopback`](#address-ipv6_loopback) | Builds the IPv6 loopback address ::1. |
| [`Endpoint_Create`](#address-endpoint_create) | Combines an address and a 16-bit host-order port. |
| [`Equal`](#address-equal) | Compares two IP address values. |

The following examples may reference a previously initialized object, such as `socket`, `connection`, `datagram`, `listener`, `poller`, `endpoint` or `error`. Instantiate and validate the appropriate state before using a fragment.

---

# Address IPv4_Create

Builds an IPv4 address from four octets.

### Syntax

#### Macro form

```c
void NETWORK_ADDRESS_FUNC(IPv4_Create)( NETWORK_ADDRESS_TYPE(TAddress) *address, uint8_t a, uint8_t b, uint8_t c, uint8_t d);
```

#### Direct form

```c
void Network_Address_IPv4_Create( Network_Address_TAddress *address, uint8_t a, uint8_t b, uint8_t c, uint8_t d);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `address` | `NETWORK_ADDRESS_TYPE(TAddress) *address` | Address object to populate or inspect. |
| `a` | `uint8_t a` | First IPv4 octet. |
| `b` | `uint8_t b` | Second IPv4 octet. |
| `c` | `uint8_t c` | Third IPv4 octet. |
| `d` | `uint8_t d` | Fourth IPv4 octet. |

---

### Return value

None.

---

### Remarks

Writes the type `NETWORK_ADDRESS_CONST(IPV4)` and the four bytes. `address == NULL` is ignored without a status.

---

### Example

```c
NETWORK_ADDRESS_TYPE(TAddress) address;
NETWORK_ADDRESS_FUNC(IPv4_Create)(&address, 192, 0, 2, 10);
```

---

# Address IPv4_Any

Builds the IPv4 wildcard address 0.0.0.0.

### Syntax

#### Macro form

```c
void NETWORK_ADDRESS_FUNC(IPv4_Any)( NETWORK_ADDRESS_TYPE(TAddress) *address);
```

#### Direct form

```c
void Network_Address_IPv4_Any( Network_Address_TAddress *address);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `address` | `NETWORK_ADDRESS_TYPE(TAddress) *address` | Address object to populate or inspect. |

---

### Return value

None.

---

### Remarks

Useful for a listening/bound endpoint, not a remote peer. Handles NULL by doing nothing.

---

### Example

```c
NETWORK_ADDRESS_TYPE(TAddress) address;
NETWORK_ADDRESS_FUNC(IPv4_Any)(&address);
```

---

# Address IPv4_Loopback

Builds the IPv4 loopback address 127.0.0.1.

### Syntax

#### Macro form

```c
void NETWORK_ADDRESS_FUNC(IPv4_Loopback)( NETWORK_ADDRESS_TYPE(TAddress) *address);
```

#### Direct form

```c
void Network_Address_IPv4_Loopback( Network_Address_TAddress *address);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `address` | `NETWORK_ADDRESS_TYPE(TAddress) *address` | Address object to populate or inspect. |

---

### Return value

None.

---

### Remarks

Useful for locally scoped TCP/UDP testing. Handles NULL without dereferencing it.

---

### Example

```c
NETWORK_ADDRESS_TYPE(TAddress) local;
NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&local);
```

---

# Address IPv6_Any

Builds the IPv6 wildcard address ::.

### Syntax

#### Macro form

```c
void NETWORK_ADDRESS_FUNC(IPv6_Any)( NETWORK_ADDRESS_TYPE(TAddress) *address);
```

#### Direct form

```c
void Network_Address_IPv6_Any( Network_Address_TAddress *address);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `address` | `NETWORK_ADDRESS_TYPE(TAddress) *address` | Address object to populate or inspect. |

---

### Return value

None.

---

### Remarks

Sets all sixteen IPv6 bytes to zero and selects the IPv6 type. Does not specify a scope ID.

---

### Example

```c
NETWORK_ADDRESS_TYPE(TAddress) address;
NETWORK_ADDRESS_FUNC(IPv6_Any)(&address);
```

---

# Address IPv6_Loopback

Builds the IPv6 loopback address ::1.

### Syntax

#### Macro form

```c
void NETWORK_ADDRESS_FUNC(IPv6_Loopback)( NETWORK_ADDRESS_TYPE(TAddress) *address);
```

#### Direct form

```c
void Network_Address_IPv6_Loopback( Network_Address_TAddress *address);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `address` | `NETWORK_ADDRESS_TYPE(TAddress) *address` | Address object to populate or inspect. |

---

### Return value

None.

---

### Remarks

Only the last IPv6 byte is 1. Handles NULL without modifying anything.

---

### Example

```c
NETWORK_ADDRESS_TYPE(TAddress) local;
NETWORK_ADDRESS_FUNC(IPv6_Loopback)(&local);
```

---

# Address Endpoint_Create

Combines an address and a 16-bit host-order port.

### Syntax

#### Macro form

```c
void NETWORK_ADDRESS_FUNC(Endpoint_Create)( NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint, const NETWORK_ADDRESS_TYPE(TAddress) *address, uint16_t port);
```

#### Direct form

```c
void Network_Address_Endpoint_Create( Network_Address_TEndpoint *endpoint, const Network_Address_TAddress *address, uint16_t port);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `endpoint` | `NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint` | IP address plus host-order port; input or output according to pointer constness. |
| `address` | `const NETWORK_ADDRESS_TYPE(TAddress) *address` | Address object to populate or inspect. |
| `port` | `uint16_t port` | Port number in host byte order. |

---

### Return value

None.

---

### Remarks

Copies the address by value into the endpoint; it does not allocate. Either pointer may be NULL, in which case it does nothing. Port 0 can request an ephemeral port when binding.

---

### Example

```c
NETWORK_ADDRESS_TYPE(TAddress) address;
NETWORK_ADDRESS_TYPE(TEndpoint) endpoint;
NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&address);
NETWORK_ADDRESS_FUNC(Endpoint_Create)(&endpoint, &address, 0);
```

---

# Address Equal

Compares two IP address values.

### Syntax

#### Macro form

```c
bool NETWORK_ADDRESS_FUNC(Equal)( const NETWORK_ADDRESS_TYPE(TAddress) *left, const NETWORK_ADDRESS_TYPE(TAddress) *right);
```

#### Direct form

```c
bool Network_Address_Equal( const Network_Address_TAddress *left, const Network_Address_TAddress *right);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `left` | `const NETWORK_ADDRESS_TYPE(TAddress) *left` | First address to compare. |
| `right` | `const NETWORK_ADDRESS_TYPE(TAddress) *right` | Second address to compare. |

---

### Return value

`bool`: true when the tested property holds, false otherwise.

---

### Remarks

Requires non-NULL pointers and equal address-family tags; compares 4 bytes for IPv4 or 16 for IPv6. **Ports are not compared** because the arguments are TAddress, not TEndpoint.

---

### Example

```c
bool equal = NETWORK_ADDRESS_FUNC(Equal)(&first, &second);
```

---

# Resolve package

Header: `Cosmeron/Modules/Network/Resolve/Resolve.h`

`TAddressVector` is a specialization of Container's Flat Vector: use `FLAT_VECTOR_FUNC(Network_Address, Init/Destroy)` to manage it. The results are IP addresses without ports, not endpoint objects.

### Types

| Type | Contents |
| --- | --- |
| `NETWORK_RESOLVE_TYPE(TAddressVector)` | Flat Vector of `NETWORK_ADDRESS_TYPE(TAddress)`; fields `.size`, `.data` and allocated storage |

### Function summary

| Function | Description |
| --- | --- |
| [`Address`](#resolve-address) | Resolves a hostname into an owning vector of unique IPv4/IPv6 addresses. |

The following examples may reference a previously initialized object, such as `socket`, `connection`, `datagram`, `listener`, `poller`, `endpoint` or `error`. Instantiate and validate the appropriate state before using a fragment.

---

# Resolve Address

Resolves a hostname into an owning vector of unique IPv4/IPv6 addresses.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_RESOLVE_FUNC(Address)( NETWORK_RESOLVE_TYPE(TAddressVector) *addresses, const char *host, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Resolve_Address( Network_Resolve_TAddressVector *addresses, const char *host, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `addresses` | `NETWORK_RESOLVE_TYPE(TAddressVector) *addresses` | Owning vector of resolved addresses; must be initialized. |
| `host` | `const char *host` | Hostname or textual IP address. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Uses `getaddrinfo` (or dynamically loaded Winsock equivalent) with `AF_UNSPEC`; it removes duplicate IP values and does not provide port numbers. Initialize the output vector before use. On success destroys/replaces its prior content; on failure preserves it. Destroy the final vector using `FLAT_VECTOR_FUNC(Network_Address, Destroy)`. Optional `error` gives the network-domain reason.

---

### Example

```c
NETWORK_RESOLVE_TYPE(TAddressVector) addresses;
FLAT_VECTOR_FUNC(Network_Address, Init)(&addresses);
NETWORK_TYPE(TError) error = NETWORK_ERROR_CONST(NONE);
OPSTATUS status = NETWORK_RESOLVE_FUNC(Address)(&addresses, "localhost", &error);
/* Inspect addresses.data[0..addresses.size) on success. */
FLAT_VECTOR_FUNC(Network_Address, Destroy)(&addresses);
```

---

# Socket package

Header: `Cosmeron/Modules/Network/Socket/Socket.h`

`TSocket` is the native-handle-owning primitive. Initialize with `SOCKET_FUNC(Init)` (not just `{0}`), then Create and finally Close. Public enums shield callers from native `AF_*`, `SOCK_*`, and protocol values.

### Types

| Type / enum | Values / ownership |
| --- | --- |
| `SOCKET_TYPE(TSocket)` | Native handle, family, kind, protocol, blocking flag |
| `SOCKET_TYPE(TFamily)` | `SOCKET_CONST(FAMILY_IPV4)`, `FAMILY_IPV6` |
| `SOCKET_TYPE(TKind)` | `SOCKET_CONST(KIND_STREAM)`, `KIND_DATAGRAM` |
| `SOCKET_TYPE(TProtocol)` | `SOCKET_CONST(PROTOCOL_DEFAULT)`, `PROTOCOL_TCP`, `PROTOCOL_UDP` |
| `SOCKET_TYPE(TShutdown)` | `SOCKET_CONST(SHUTDOWN_READ)`, `SHUTDOWN_WRITE`, `SHUTDOWN_BOTH` |

### Function summary

| Function | Description |
| --- | --- |
| [`Init`](#socket-init) | Initializes a socket object with an invalid native handle. |
| [`Create`](#socket-create) | Creates a native socket with the requested family, kind and protocol. |
| [`Close`](#socket-close) | Closes a native socket and invalidates its handle. |
| [`Bind`](#socket-bind) | Binds a native socket to an IP endpoint. |
| [`Listen`](#socket-listen) | Places a bound stream socket into listening mode. |
| [`Accept`](#socket-accept) | Accepts a client connection from a listening socket. |
| [`Connect`](#socket-connect) | Connects a socket to a remote endpoint. |
| [`Send`](#socket-send) | Sends up to the specified number of bytes. |
| [`Receive`](#socket-receive) | Receives bytes from a connected socket. |
| [`SendTo`](#socket-sendto) | Sends a datagram to a specified endpoint. |
| [`ReceiveFrom`](#socket-receivefrom) | Receives a datagram and optionally reports the sender endpoint. |
| [`Shutdown`](#socket-shutdown) | Shuts down one or both directions of a connected socket. |
| [`SetBlocking`](#socket-setblocking) | Selects blocking or nonblocking socket operation. |
| [`SetReuseAddress`](#socket-setreuseaddress) | Enables or disables SO_REUSEADDR. |
| [`SetKeepAlive`](#socket-setkeepalive) | Enables or disables SO_KEEPALIVE. |
| [`SetNoDelay`](#socket-setnodelay) | Enables or disables TCP_NODELAY. |

The following examples may reference a previously initialized object, such as `socket`, `connection`, `datagram`, `listener`, `poller`, `endpoint` or `error`. Instantiate and validate the appropriate state before using a fragment.

---

# Socket Init

Initializes a socket object with an invalid native handle.

### Syntax

#### Macro form

```c
void SOCKET_FUNC(Init)(SOCKET_TYPE(TSocket) *socket);
```

#### Direct form

```c
void Network_Socket_Init(Network_Socket_TSocket *socket);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `socket` | `SOCKET_TYPE(TSocket) *socket` | Initialized low-level socket object. |

---

### Return value

None.

---

### Remarks

Must precede any Create/Close or operation that inspects the handle. Defaults to IPv4, STREAM, PROTOCOL_DEFAULT and blocking=true; NULL is ignored. An uninitialized `{0}` struct alone is not a portable substitute for Init.

---

### Example

```c
SOCKET_TYPE(TSocket) socket;
SOCKET_FUNC(Init)(&socket);
```

---

# Socket Create

Creates a native socket with the requested family, kind and protocol.

### Syntax

#### Macro form

```c
OPSTATUS SOCKET_FUNC(Create)( SOCKET_TYPE(TSocket) *socket, SOCKET_TYPE(TFamily) family, SOCKET_TYPE(TKind) kind, SOCKET_TYPE(TProtocol) protocol, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Socket_Create( Network_Socket_TSocket *socket, Network_Socket_TFamily family, Network_Socket_TKind kind, Network_Socket_TProtocol protocol, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `socket` | `SOCKET_TYPE(TSocket) *socket` | Initialized low-level socket object. |
| `family` | `SOCKET_TYPE(TFamily) family` | IP address family (IPv4 or IPv6). |
| `kind` | `SOCKET_TYPE(TKind) kind` | Stream (TCP-like) or datagram (UDP-like) socket. |
| `protocol` | `SOCKET_TYPE(TProtocol) protocol` | DEFAULT, TCP or UDP protocol enumeration. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Accepted families: FAMILY_IPV4/FAMILY_IPV6; kinds: KIND_STREAM/KIND_DATAGRAM; protocols: PROTOCOL_DEFAULT/TCP/UDP. Rejects incompatible TCP/datagram or UDP/stream pairs. Closes an already-open handle before recreating; leaves blocking as configured by Init (native sockets are initially blocking). Runtime socket setup happens automatically on Windows.

---

### Example

```c
SOCKET_TYPE(TSocket) socket;
SOCKET_FUNC(Init)(&socket);
NETWORK_TYPE(TError) error;
OPSTATUS status = SOCKET_FUNC(Create)(&socket, SOCKET_CONST(FAMILY_IPV4),
    SOCKET_CONST(KIND_STREAM), SOCKET_CONST(PROTOCOL_TCP), &error);
SOCKET_FUNC(Close)(&socket);
```

---

# Socket Close

Closes a native socket and invalidates its handle.

### Syntax

#### Macro form

```c
void SOCKET_FUNC(Close)(SOCKET_TYPE(TSocket) *socket);
```

#### Direct form

```c
void Network_Socket_Close(Network_Socket_TSocket *socket);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `socket` | `SOCKET_TYPE(TSocket) *socket` | Initialized low-level socket object. |

---

### Return value

None.

---

### Remarks

Safe with NULL or an already-invalid handle. Does not release a higher-level Connection's hostname or otherwise reset the entire structure.

---

### Example

```c
SOCKET_FUNC(Close)(&socket);
```

---

# Socket Bind

Binds a native socket to an IP endpoint.

### Syntax

#### Macro form

```c
OPSTATUS SOCKET_FUNC(Bind)( SOCKET_TYPE(TSocket) *socket, const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Socket_Bind( Network_Socket_TSocket *socket, const Network_Address_TEndpoint *endpoint, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `socket` | `SOCKET_TYPE(TSocket) *socket` | Initialized low-level socket object. |
| `endpoint` | `const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint` | IP address plus host-order port; input or output according to pointer constness. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

The endpoint family should match the socket. A port of zero requests OS-assigned ephemeral port. OS errors are translated into `NETWORK_TYPE(TError)`.

---

### Example

```c
OPSTATUS status = SOCKET_FUNC(Bind)(&socket, &endpoint, &error);
```

---

# Socket Listen

Places a bound stream socket into listening mode.

### Syntax

#### Macro form

```c
OPSTATUS SOCKET_FUNC(Listen)( SOCKET_TYPE(TSocket) *socket, int backlog, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Socket_Listen( Network_Socket_TSocket *socket, int backlog, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `socket` | `SOCKET_TYPE(TSocket) *socket` | Initialized low-level socket object. |
| `backlog` | `int backlog` | Native pending-connection backlog. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

The backlog is an `int` passed to the native API. Use `Accept` after success; this is not a UDP operation.

---

### Example

```c
OPSTATUS status = SOCKET_FUNC(Listen)(&socket, 128, &error);
```

---

# Socket Accept

Accepts a client connection from a listening socket.

### Syntax

#### Macro form

```c
OPSTATUS SOCKET_FUNC(Accept)( SOCKET_TYPE(TSocket) *socket, SOCKET_TYPE(TSocket) *client, NETWORK_ADDRESS_TYPE(TEndpoint) *peer, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Socket_Accept( Network_Socket_TSocket *socket, Network_Socket_TSocket *client, Network_Address_TEndpoint *peer, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `socket` | `SOCKET_TYPE(TSocket) *socket` | Initialized low-level socket object. |
| `client` | `SOCKET_TYPE(TSocket) *client` | Initialized destination socket for an accepted client. |
| `peer` | `NETWORK_ADDRESS_TYPE(TEndpoint) *peer` | Optional output remote endpoint; may be NULL. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

`client` must have been initialized using Socket Init; an existing client handle is closed on success. `peer` may be NULL. Accepted socket inherits the configured blocking preference and the source socket's family/kind/protocol metadata.

---

### Example

```c
SOCKET_TYPE(TSocket) client;
SOCKET_FUNC(Init)(&client);
OPSTATUS status = SOCKET_FUNC(Accept)(&listenerSocket, &client, &peer, &error);
```

---

# Socket Connect

Connects a socket to a remote endpoint.

### Syntax

#### Macro form

```c
OPSTATUS SOCKET_FUNC(Connect)( SOCKET_TYPE(TSocket) *socket, const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Socket_Connect( Network_Socket_TSocket *socket, const Network_Address_TEndpoint *endpoint, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `socket` | `SOCKET_TYPE(TSocket) *socket` | Initialized low-level socket object. |
| `endpoint` | `const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint` | IP address plus host-order port; input or output according to pointer constness. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

On a nonblocking native socket, an in-progress connection is returned as a non-success status with `NETWORK_ERROR_CONST(WOULD_BLOCK)`; use readiness and a `SO_ERROR` check rather than interpreting this as immediate success. `Connection_ConnectFor` provides a higher-level timed alternative.

---

### Example

```c
OPSTATUS status = SOCKET_FUNC(Connect)(&socket, &remote, &error);
```

---

# Socket Send

Sends up to the specified number of bytes.

### Syntax

#### Macro form

```c
OPSTATUS SOCKET_FUNC(Send)( SOCKET_TYPE(TSocket) *socket, const void *data, size_t size, size_t *sent, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Socket_Send( Network_Socket_TSocket *socket, const void *data, size_t size, size_t *sent, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `socket` | `SOCKET_TYPE(TSocket) *socket` | Initialized low-level socket object. |
| `data` | `const void *data` | Bytes to transmit. |
| `size` | `size_t size` | Byte count to send, read, or transfer. |
| `sent` | `size_t *sent` | Writable number of bytes sent; see optionality for SendTo. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

`sent` is required; it is set to the number of bytes actually sent on success, which can be less than `size`. A zero byte-count allows NULL data. On Windows one native call is capped to INT_MAX bytes. Retry remaining data appropriately.

---

### Example

```c
const char payload[] = "hello";
size_t sent = 0;
OPSTATUS status = SOCKET_FUNC(Send)(&socket, payload, sizeof(payload), &sent, &error);
```

---

# Socket Receive

Receives bytes from a connected socket.

### Syntax

#### Macro form

```c
OPSTATUS SOCKET_FUNC(Receive)( SOCKET_TYPE(TSocket) *socket, void *data, size_t capacity, size_t *received, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Socket_Receive( Network_Socket_TSocket *socket, void *data, size_t capacity, size_t *received, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `socket` | `SOCKET_TYPE(TSocket) *socket` | Initialized low-level socket object. |
| `data` | `void *data` | Bytes to transmit. |
| `capacity` | `size_t capacity` | Available capacity of destination buffer, in bytes. |
| `received` | `size_t *received` | Writable number of bytes received. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

`received` is required. A capacity of 0 returns success with 0 bytes without a native receive; a positive-capacity receive returning 0 reports `CONNECTION_CLOSED` via `error` and GENERIC_ERROR via OPSTATUS. A single call may read fewer bytes than requested.

---

### Example

```c
char buffer[256]; size_t received = 0;
OPSTATUS status = SOCKET_FUNC(Receive)(&socket, buffer, sizeof(buffer), &received, &error);
```

---

# Socket SendTo

Sends a datagram to a specified endpoint.

### Syntax

#### Macro form

```c
OPSTATUS SOCKET_FUNC(SendTo)( SOCKET_TYPE(TSocket) *socket, const void *data, size_t size, const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint, size_t *sent, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Socket_SendTo( Network_Socket_TSocket *socket, const void *data, size_t size, const Network_Address_TEndpoint *endpoint, size_t *sent, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `socket` | `SOCKET_TYPE(TSocket) *socket` | Initialized low-level socket object. |
| `data` | `const void *data` | Bytes to transmit. |
| `size` | `size_t size` | Byte count to send, read, or transfer. |
| `endpoint` | `const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint` | IP address plus host-order port; input or output according to pointer constness. |
| `sent` | `size_t *sent` | Optional number of bytes sent; may be NULL. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Intended for datagram sockets. `sent` is optional at this low level, unlike `Datagram_SendTo`. The endpoint is required; datagram size constraints and native errors remain platform-dependent.

---

### Example

```c
const char message[] = "ping"; size_t sent = 0;
OPSTATUS status = SOCKET_FUNC(SendTo)(&socket, message, sizeof(message),
    &endpoint, &sent, &error);
```

---

# Socket ReceiveFrom

Receives a datagram and optionally reports the sender endpoint.

### Syntax

#### Macro form

```c
OPSTATUS SOCKET_FUNC(ReceiveFrom)( SOCKET_TYPE(TSocket) *socket, void *data, size_t capacity, NETWORK_ADDRESS_TYPE(TEndpoint) *sender, size_t *received, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Socket_ReceiveFrom( Network_Socket_TSocket *socket, void *data, size_t capacity, Network_Address_TEndpoint *sender, size_t *received, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `socket` | `SOCKET_TYPE(TSocket) *socket` | Initialized low-level socket object. |
| `data` | `void *data` | Bytes to transmit. |
| `capacity` | `size_t capacity` | Available capacity of destination buffer, in bytes. |
| `sender` | `NETWORK_ADDRESS_TYPE(TEndpoint) *sender` | Optional output UDP sender endpoint; may be NULL. |
| `received` | `size_t *received` | Writable number of bytes received. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

`received` is required, while `sender` may be NULL. A successful zero-byte UDP datagram is permitted; do not confuse this with stream EOF behavior.

---

### Example

```c
char buffer[512]; size_t received = 0;
NETWORK_ADDRESS_TYPE(TEndpoint) sender;
OPSTATUS status = SOCKET_FUNC(ReceiveFrom)(&socket, buffer,
    sizeof(buffer), &sender, &received, &error);
```

---

# Socket Shutdown

Shuts down one or both directions of a connected socket.

### Syntax

#### Macro form

```c
OPSTATUS SOCKET_FUNC(Shutdown)( SOCKET_TYPE(TSocket) *socket, SOCKET_TYPE(TShutdown) direction, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Socket_Shutdown( Network_Socket_TSocket *socket, Network_Socket_TShutdown direction, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `socket` | `SOCKET_TYPE(TSocket) *socket` | Initialized low-level socket object. |
| `direction` | `SOCKET_TYPE(TShutdown) direction` | Shutdown read, write or both directions. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Directions: SHUTDOWN_READ, SHUTDOWN_WRITE, SHUTDOWN_BOTH. This does not close the native handle; call Close afterward. Invalid directions return INVALID_ARGUMENT.

---

### Example

```c
OPSTATUS status = SOCKET_FUNC(Shutdown)(&socket,
    SOCKET_CONST(SHUTDOWN_WRITE), &error);
```

---

# Socket SetBlocking

Selects blocking or nonblocking socket operation.

### Syntax

#### Macro form

```c
OPSTATUS SOCKET_FUNC(SetBlocking)( SOCKET_TYPE(TSocket) *socket, bool blocking, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Socket_SetBlocking( Network_Socket_TSocket *socket, bool blocking, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `socket` | `SOCKET_TYPE(TSocket) *socket` | Initialized low-level socket object. |
| `blocking` | `bool blocking` | True for blocking operations, false for nonblocking. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Sets platform socket mode using `ioctlsocket(FIONBIO)` on Windows or `fcntl(O_NONBLOCK)` on POSIX. Updates the stored `blocking` field only after native success. An unopened handle reports NOT_CONNECTED in the error domain.

---

### Example

```c
OPSTATUS status = SOCKET_FUNC(SetBlocking)(&socket, false, &error);
```

---

# Socket SetReuseAddress

Enables or disables SO_REUSEADDR.

### Syntax

#### Macro form

```c
OPSTATUS SOCKET_FUNC(SetReuseAddress)( SOCKET_TYPE(TSocket) *socket, bool enabled, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Socket_SetReuseAddress( Network_Socket_TSocket *socket, bool enabled, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `socket` | `SOCKET_TYPE(TSocket) *socket` | Initialized low-level socket object. |
| `enabled` | `bool enabled` | Boolean setting for the requested native socket option. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Maps to native `setsockopt`; native behavior varies by OS. Does not guarantee SO_REUSEPORT semantics.

---

### Example

```c
OPSTATUS status = SOCKET_FUNC(SetReuseAddress)(&socket, true, &error);
```

---

# Socket SetKeepAlive

Enables or disables SO_KEEPALIVE.

### Syntax

#### Macro form

```c
OPSTATUS SOCKET_FUNC(SetKeepAlive)( SOCKET_TYPE(TSocket) *socket, bool enabled, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Socket_SetKeepAlive( Network_Socket_TSocket *socket, bool enabled, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `socket` | `SOCKET_TYPE(TSocket) *socket` | Initialized low-level socket object. |
| `enabled` | `bool enabled` | Boolean setting for the requested native socket option. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Native OS keep-alive timing/interval configuration is not exposed here.

---

### Example

```c
OPSTATUS status = SOCKET_FUNC(SetKeepAlive)(&socket, true, &error);
```

---

# Socket SetNoDelay

Enables or disables TCP_NODELAY.

### Syntax

#### Macro form

```c
OPSTATUS SOCKET_FUNC(SetNoDelay)( SOCKET_TYPE(TSocket) *socket, bool enabled, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Socket_SetNoDelay( Network_Socket_TSocket *socket, bool enabled, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `socket` | `SOCKET_TYPE(TSocket) *socket` | Initialized low-level socket object. |
| `enabled` | `bool enabled` | Boolean setting for the requested native socket option. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Intended for TCP stream sockets; suppresses Nagle-style buffering where supported. Applied through native `setsockopt`.

---

### Example

```c
OPSTATUS status = SOCKET_FUNC(SetNoDelay)(&socket, true, &error);
```

---

# Connection package

Header: `Cosmeron/Modules/Network/Connection/Connection.h`

`TConnection` owns an embedded Socket and a dynamically allocated copy of its target hostname. `Init` → `Connect`/`ConnectFor` → I/O → `Destroy` is the standard path. `Close` retains host metadata for reconnecting.

### Types

| Type / state | Contents |
| --- | --- |
| `NETWORK_CONNECTION_TYPE(TConnection)` | `.socket`, `.state`, `.localEndpoint`, `.remoteEndpoint`, owned `.host`, `.port`, `.blocking`, `.connectTimeout` |
| `NETWORK_CONNECTION_TYPE(TState)` | `NETWORK_CONNECTION_CONST(IDLE/CONNECTING/CONNECTED/CLOSING/CLOSED/FAILED)` |
| `NETWORK_CONNECTION_DEFAULT_TIMEOUT_SECONDS` | `30` |

### Function summary

| Function | Description |
| --- | --- |
| [`Init`](#connection-init) | Initializes a high-level TCP connection object. |
| [`Connect`](#connection-connect) | Connects to a hostname and port using the default timeout. |
| [`ConnectFor`](#connection-connectfor) | Connects with an explicit overall timeout. |
| [`Reconnect`](#connection-reconnect) | Reconnects using the stored hostname, port and timeout. |
| [`Read`](#connection-read) | Reads up to a given byte capacity from an established TCP connection. |
| [`ReadExact`](#connection-readexact) | Reads exactly `size` bytes, looping until complete or an error. |
| [`Write`](#connection-write) | Writes up to `size` bytes using one TCP send. |
| [`WriteAll`](#connection-writeall) | Writes exactly `size` bytes by looping over Write. |
| [`Shutdown`](#connection-shutdown) | Shuts down TCP read, write, or both directions. |
| [`SetBlocking`](#connection-setblocking) | Configures whether future I/O calls use blocking mode. |
| [`Close`](#connection-close) | Closes the underlying TCP socket but retains reconnect metadata. |
| [`Destroy`](#connection-destroy) | Closes a connection and releases its owned hostname. |
| [`State`](#connection-state) | Reads the current high-level connection state. |
| [`LocalEndpoint`](#connection-localendpoint) | Returns the active TCP connection's local endpoint. |
| [`RemoteEndpoint`](#connection-remoteendpoint) | Returns the connected remote endpoint. |

The following examples may reference a previously initialized object, such as `socket`, `connection`, `datagram`, `listener`, `poller`, `endpoint` or `error`. Instantiate and validate the appropriate state before using a fragment.

---

# Connection Init

Initializes a high-level TCP connection object.

### Syntax

#### Macro form

```c
void NETWORK_CONNECTION_FUNC(Init)( NETWORK_CONNECTION_TYPE(TConnection) *connection);
```

#### Direct form

```c
void Network_Connection_Init( Network_Connection_TConnection *connection);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `connection` | `NETWORK_CONNECTION_TYPE(TConnection) *connection` | High-level TCP connection object. |

---

### Return value

None.

---

### Remarks

Sets state IDLE, blocking=true and default connect timeout to 30 seconds; initializes the embedded Socket. Call before Connect/Close/Destroy. Reinitializing a live connection without Destroy can leak the owned host string.

---

### Example

```c
NETWORK_CONNECTION_TYPE(TConnection) connection;
NETWORK_CONNECTION_FUNC(Init)(&connection);
```

---

# Connection Connect

Connects to a hostname and port using the default timeout.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_CONNECTION_FUNC(Connect)( NETWORK_CONNECTION_TYPE(TConnection) *connection, const char *host, uint16_t port, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Connection_Connect( Network_Connection_TConnection *connection, const char *host, uint16_t port, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `connection` | `NETWORK_CONNECTION_TYPE(TConnection) *connection` | High-level TCP connection object. |
| `host` | `const char *host` | Hostname or textual IP address. |
| `port` | `uint16_t port` | Port number in host byte order. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Equivalent to ConnectFor with 30 seconds. Resolves IPv4 and IPv6 and attempts addresses in order using a shared deadline. Stores an owned copy of host for later Reconnect; state becomes CONNECTED on success or FAILED on failure.

---

### Example

```c
OPSTATUS status = NETWORK_CONNECTION_FUNC(Connect)(
    &connection, "127.0.0.1", 8080, &error);
```

---

# Connection ConnectFor

Connects with an explicit overall timeout.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_CONNECTION_FUNC(ConnectFor)( NETWORK_CONNECTION_TYPE(TConnection) *connection, const char *host, uint16_t port, CHRONOMETRY_TYPE(Duration) timeout, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Connection_ConnectFor( Network_Connection_TConnection *connection, const char *host, uint16_t port, Chronometry_Duration timeout, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `connection` | `NETWORK_CONNECTION_TYPE(TConnection) *connection` | High-level TCP connection object. |
| `host` | `const char *host` | Hostname or textual IP address. |
| `port` | `uint16_t port` | Port number in host byte order. |
| `timeout` | `CHRONOMETRY_TYPE(Duration) timeout` | Nonnegative Chronometry Duration (nanoseconds). |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

`timeout` is a nonnegative Chronometry Duration in nanoseconds. Uses a temporary nonblocking connect and restores the configured blocking preference, waiting on readiness/`SO_ERROR`. Tries resolved addresses within the deadline. Zero timeout may fail quickly. The call itself can block up to the deadline even when subsequent I/O is nonblocking.

---

### Example

```c
CHRONOMETRY_TYPE(Duration) timeout = DURATION_FUNC(FromNanoseconds)(
    INT64_C(2) * CHRONOMETRY_DURATION_NANOSECONDS_PER_SECOND);
OPSTATUS status = NETWORK_CONNECTION_FUNC(ConnectFor)(
    &connection, "localhost", 8080, timeout, &error);
```

---

# Connection Reconnect

Reconnects using the stored hostname, port and timeout.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_CONNECTION_FUNC(Reconnect)( NETWORK_CONNECTION_TYPE(TConnection) *connection, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Connection_Reconnect( Network_Connection_TConnection *connection, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `connection` | `NETWORK_CONNECTION_TYPE(TConnection) *connection` | High-level TCP connection object. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Requires a host established by an earlier Connect/ConnectFor attempt. Copies the host before reconnecting to avoid lifetime aliasing. Uses the original connection timeout and resets state based on outcome.

---

### Example

```c
OPSTATUS status = NETWORK_CONNECTION_FUNC(Reconnect)(&connection, &error);
```

---

# Connection Read

Reads up to a given byte capacity from an established TCP connection.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_CONNECTION_FUNC(Read)( NETWORK_CONNECTION_TYPE(TConnection) *connection, void *buffer, size_t capacity, size_t *bytesRead, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Connection_Read( Network_Connection_TConnection *connection, void *buffer, size_t capacity, size_t *bytesRead, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `connection` | `NETWORK_CONNECTION_TYPE(TConnection) *connection` | High-level TCP connection object. |
| `buffer` | `void *buffer` | Caller-owned writable byte buffer. |
| `capacity` | `size_t capacity` | Available capacity of destination buffer, in bytes. |
| `bytesRead` | `size_t *bytesRead` | Required output number of bytes actually read. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

One socket receive; returns bytes through `bytesRead` and can complete short. Requires state CONNECTED. CONNECTION_CLOSED transitions state to CLOSED; most other errors transition to FAILED, except WOULD_BLOCK which leaves the connected state.

---

### Example

```c
char buffer[256]; size_t read = 0;
OPSTATUS status = NETWORK_CONNECTION_FUNC(Read)(&connection,
    buffer, sizeof(buffer), &read, &error);
```

---

# Connection ReadExact

Reads exactly `size` bytes, looping until complete or an error.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_CONNECTION_FUNC(ReadExact)( NETWORK_CONNECTION_TYPE(TConnection) *connection, void *buffer, size_t size, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Connection_ReadExact( Network_Connection_TConnection *connection, void *buffer, size_t size, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `connection` | `NETWORK_CONNECTION_TYPE(TConnection) *connection` | High-level TCP connection object. |
| `buffer` | `void *buffer` | Caller-owned writable byte buffer. |
| `size` | `size_t size` | Byte count to send, read, or transfer. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Useful for blocking connections and fixed-size framing. If an error occurs after a partial read, the buffer may already contain data; the API does not return a partial byte count. **Do not use naively with nonblocking I/O**, since WOULD_BLOCK can interrupt the sequence.

---

### Example

```c
char header[8];
OPSTATUS status = NETWORK_CONNECTION_FUNC(ReadExact)(
    &connection, header, sizeof(header), &error);
```

---

# Connection Write

Writes up to `size` bytes using one TCP send.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_CONNECTION_FUNC(Write)( NETWORK_CONNECTION_TYPE(TConnection) *connection, const void *data, size_t size, size_t *bytesWritten, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Connection_Write( Network_Connection_TConnection *connection, const void *data, size_t size, size_t *bytesWritten, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `connection` | `NETWORK_CONNECTION_TYPE(TConnection) *connection` | High-level TCP connection object. |
| `data` | `const void *data` | Bytes to transmit. |
| `size` | `size_t size` | Byte count to send, read, or transfer. |
| `bytesWritten` | `size_t *bytesWritten` | Required output number of bytes actually written. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

The output `bytesWritten` is mandatory; even on success a short write is possible. Requires state CONNECTED. WOULD_BLOCK leaves the connection state intact for a later retry.

---

### Example

```c
const char payload[] = "hello"; size_t count = 0;
OPSTATUS status = NETWORK_CONNECTION_FUNC(Write)(&connection,
    payload, sizeof(payload), &count, &error);
```

---

# Connection WriteAll

Writes exactly `size` bytes by looping over Write.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_CONNECTION_FUNC(WriteAll)( NETWORK_CONNECTION_TYPE(TConnection) *connection, const void *data, size_t size, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Connection_WriteAll( Network_Connection_TConnection *connection, const void *data, size_t size, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `connection` | `NETWORK_CONNECTION_TYPE(TConnection) *connection` | High-level TCP connection object. |
| `data` | `const void *data` | Bytes to transmit. |
| `size` | `size_t size` | Byte count to send, read, or transfer. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Useful for blocking I/O. If a failure happens after a partial write, some bytes were already sent and the function does not return their count. A zero-byte successful send is treated as CONNECTION_CLOSED. Do not assume atomicity or retry the whole payload blindly.

---

### Example

```c
const char payload[] = "hello";
OPSTATUS status = NETWORK_CONNECTION_FUNC(WriteAll)(
    &connection, payload, sizeof(payload), &error);
```

---

# Connection Shutdown

Shuts down TCP read, write, or both directions.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_CONNECTION_FUNC(Shutdown)( NETWORK_CONNECTION_TYPE(TConnection) *connection, SOCKET_TYPE(TShutdown) direction, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Connection_Shutdown( Network_Connection_TConnection *connection, Network_Socket_TShutdown direction, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `connection` | `NETWORK_CONNECTION_TYPE(TConnection) *connection` | High-level TCP connection object. |
| `direction` | `SOCKET_TYPE(TShutdown) direction` | Shutdown read, write or both directions. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Delegates to embedded Socket_Shutdown. Does not release the connection or its host copy; use Close/Destroy for cleanup. Errors can mark state FAILED.

---

### Example

```c
OPSTATUS status = NETWORK_CONNECTION_FUNC(Shutdown)(
    &connection, SOCKET_CONST(SHUTDOWN_WRITE), &error);
```

---

# Connection SetBlocking

Configures whether future I/O calls use blocking mode.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_CONNECTION_FUNC(SetBlocking)( NETWORK_CONNECTION_TYPE(TConnection) *connection, bool blocking, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Connection_SetBlocking( Network_Connection_TConnection *connection, bool blocking, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `connection` | `NETWORK_CONNECTION_TYPE(TConnection) *connection` | High-level TCP connection object. |
| `blocking` | `bool blocking` | True for blocking operations, false for nonblocking. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

If a socket handle exists, changes its native mode; otherwise records the preference for a future connection. A failure other than WOULD_BLOCK can transition the connection to FAILED.

---

### Example

```c
OPSTATUS status = NETWORK_CONNECTION_FUNC(SetBlocking)(
    &connection, false, &error);
```

---

# Connection Close

Closes the underlying TCP socket but retains reconnect metadata.

### Syntax

#### Macro form

```c
void NETWORK_CONNECTION_FUNC(Close)( NETWORK_CONNECTION_TYPE(TConnection) *connection);
```

#### Direct form

```c
void Network_Connection_Close( Network_Connection_TConnection *connection);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `connection` | `NETWORK_CONNECTION_TYPE(TConnection) *connection` | High-level TCP connection object. |

---

### Return value

None.

---

### Remarks

Sets state CLOSED and invalidates the socket handle. It deliberately does **not** free the owned host string; Destroy does that. NULL is ignored.

---

### Example

```c
NETWORK_CONNECTION_FUNC(Close)(&connection);
```

---

# Connection Destroy

Closes a connection and releases its owned hostname.

### Syntax

#### Macro form

```c
void NETWORK_CONNECTION_FUNC(Destroy)( NETWORK_CONNECTION_TYPE(TConnection) *connection);
```

#### Direct form

```c
void Network_Connection_Destroy( Network_Connection_TConnection *connection);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `connection` | `NETWORK_CONNECTION_TYPE(TConnection) *connection` | High-level TCP connection object. |

---

### Return value

None.

---

### Remarks

Calls Close and frees the host string. It should be used for final lifecycle cleanup; safe for NULL. Initializing a new connection afterward is preferable to reusing a destroyed instance without reset.

---

### Example

```c
NETWORK_CONNECTION_FUNC(Destroy)(&connection);
```

---

# Connection State

Reads the current high-level connection state.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_CONNECTION_FUNC(State)( const NETWORK_CONNECTION_TYPE(TConnection) *connection, NETWORK_CONNECTION_TYPE(TState) *outState);
```

#### Direct form

```c
OPSTATUS Network_Connection_State( const Network_Connection_TConnection *connection, Network_Connection_TState *outState);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `connection` | `const NETWORK_CONNECTION_TYPE(TConnection) *connection` | High-level TCP connection object. |
| `outState` | `NETWORK_CONNECTION_TYPE(TState) *outState` | Writable high-level connection state result. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Writes IDLE, CONNECTING, CONNECTED, CLOSING, CLOSED or FAILED into a required output pointer. This is a snapshot, not a future event subscription.

---

### Example

```c
NETWORK_CONNECTION_TYPE(TState) state;
OPSTATUS status = NETWORK_CONNECTION_FUNC(State)(&connection, &state);
```

---

# Connection LocalEndpoint

Returns the active TCP connection's local endpoint.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_CONNECTION_FUNC(LocalEndpoint)( const NETWORK_CONNECTION_TYPE(TConnection) *connection, NETWORK_ADDRESS_TYPE(TEndpoint) *outEndpoint);
```

#### Direct form

```c
OPSTATUS Network_Connection_LocalEndpoint( const Network_Connection_TConnection *connection, Network_Address_TEndpoint *outEndpoint);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `connection` | `const NETWORK_CONNECTION_TYPE(TConnection) *connection` | High-level TCP connection object. |
| `outEndpoint` | `NETWORK_ADDRESS_TYPE(TEndpoint) *outEndpoint` | Writable endpoint output. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Requires state CONNECTED; otherwise returns NOT_AVAILABLE. Endpoint contains address plus the actual local port. Both pointers are required.

---

### Example

```c
NETWORK_ADDRESS_TYPE(TEndpoint) local;
OPSTATUS status = NETWORK_CONNECTION_FUNC(LocalEndpoint)(&connection, &local);
```

---

# Connection RemoteEndpoint

Returns the connected remote endpoint.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_CONNECTION_FUNC(RemoteEndpoint)( const NETWORK_CONNECTION_TYPE(TConnection) *connection, NETWORK_ADDRESS_TYPE(TEndpoint) *outEndpoint);
```

#### Direct form

```c
OPSTATUS Network_Connection_RemoteEndpoint( const Network_Connection_TConnection *connection, Network_Address_TEndpoint *outEndpoint);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `connection` | `const NETWORK_CONNECTION_TYPE(TConnection) *connection` | High-level TCP connection object. |
| `outEndpoint` | `NETWORK_ADDRESS_TYPE(TEndpoint) *outEndpoint` | Writable endpoint output. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Requires state CONNECTED; otherwise returns NOT_AVAILABLE. Returns a copied TEndpoint, not a borrowed reference.

---

### Example

```c
NETWORK_ADDRESS_TYPE(TEndpoint) remote;
OPSTATUS status = NETWORK_CONNECTION_FUNC(RemoteEndpoint)(&connection, &remote);
```

---

# Datagram package

Header: `Cosmeron/Modules/Network/Datagram/Datagram.h`

`TDatagram` owns one UDP Socket and optionally stores a connected peer endpoint. Connected UDP is still datagram-oriented and does not guarantee delivery or ordering.

### Types

| Type | Contents |
| --- | --- |
| `NETWORK_DATAGRAM_TYPE(TDatagram)` | Owned `.socket`; cached `.localEndpoint`, `.peerEndpoint`; `.blocking`, `.hasPeer` |

### Function summary

| Function | Description |
| --- | --- |
| [`Init`](#datagram-init) | Initializes the UDP object with no open socket. |
| [`Open`](#datagram-open) | Opens an IPv4 or IPv6 UDP socket. |
| [`Bind`](#datagram-bind) | Binds a UDP socket to a local endpoint. |
| [`SetPeer`](#datagram-setpeer) | Associates a UDP peer through a native datagram connect. |
| [`SendTo`](#datagram-sendto) | Sends a datagram to an explicitly named endpoint. |
| [`ReceiveFrom`](#datagram-receivefrom) | Receives a datagram with optional source endpoint. |
| [`Send`](#datagram-send) | Sends through the associated UDP peer. |
| [`Receive`](#datagram-receive) | Reads datagrams from the associated UDP peer. |
| [`SetBlocking`](#datagram-setblocking) | Sets the UDP socket's native blocking mode. |
| [`ClearPeer`](#datagram-clearpeer) | Clears the previously configured UDP peer association. |
| [`Close`](#datagram-close) | Closes UDP socket and forgets that a peer was configured. |
| [`HasPeer`](#datagram-haspeer) | Tests whether a UDP peer is configured. |
| [`LocalEndpoint`](#datagram-localendpoint) | Reports the cached local UDP endpoint. |
| [`PeerEndpoint`](#datagram-peerendpoint) | Reports the configured UDP peer endpoint. |

The following examples may reference a previously initialized object, such as `socket`, `connection`, `datagram`, `listener`, `poller`, `endpoint` or `error`. Instantiate and validate the appropriate state before using a fragment.

---

# Datagram Init

Initializes the UDP object with no open socket.

### Syntax

#### Macro form

```c
void NETWORK_DATAGRAM_FUNC(Init)( NETWORK_DATAGRAM_TYPE(TDatagram) *datagram);
```

#### Direct form

```c
void Network_Datagram_Init( Network_Datagram_TDatagram *datagram);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `datagram` | `NETWORK_DATAGRAM_TYPE(TDatagram) *datagram` | Initialized UDP datagram object. |

---

### Return value

None.

---

### Remarks

Sets blocking=true, clears `hasPeer`, initializes the embedded Socket with its invalid handle. Call before other methods.

---

### Example

```c
NETWORK_DATAGRAM_TYPE(TDatagram) datagram;
NETWORK_DATAGRAM_FUNC(Init)(&datagram);
```

---

# Datagram Open

Opens an IPv4 or IPv6 UDP socket.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_DATAGRAM_FUNC(Open)( NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, SOCKET_TYPE(TFamily) family, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Datagram_Open( Network_Datagram_TDatagram *datagram, Network_Socket_TFamily family, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `datagram` | `NETWORK_DATAGRAM_TYPE(TDatagram) *datagram` | Initialized UDP datagram object. |
| `family` | `SOCKET_TYPE(TFamily) family` | IP address family (IPv4 or IPv6). |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Closes any previous socket and creates a new KIND_DATAGRAM/PROTOCOL_UDP socket. Sets blocking=true; family is FAMILY_IPV4 or FAMILY_IPV6.

---

### Example

```c
OPSTATUS status = NETWORK_DATAGRAM_FUNC(Open)(
    &datagram, SOCKET_CONST(FAMILY_IPV4), &error);
```

---

# Datagram Bind

Binds a UDP socket to a local endpoint.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_DATAGRAM_FUNC(Bind)( NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Datagram_Bind( Network_Datagram_TDatagram *datagram, const Network_Address_TEndpoint *endpoint, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `datagram` | `NETWORK_DATAGRAM_TYPE(TDatagram) *datagram` | Initialized UDP datagram object. |
| `endpoint` | `const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint` | IP address plus host-order port; input or output according to pointer constness. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

If unopened, it automatically opens a UDP socket with the endpoint's address family. A mismatched already-open family reports ADDRESS_FAMILY_UNSUPPORTED. On success updates stored localEndpoint from the OS.

---

### Example

```c
OPSTATUS status = NETWORK_DATAGRAM_FUNC(Bind)(&datagram, &endpoint, &error);
```

---

# Datagram SetPeer

Associates a UDP peer through a native datagram connect.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_DATAGRAM_FUNC(SetPeer)( NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Datagram_SetPeer( Network_Datagram_TDatagram *datagram, const Network_Address_TEndpoint *endpoint, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `datagram` | `NETWORK_DATAGRAM_TYPE(TDatagram) *datagram` | Initialized UDP datagram object. |
| `endpoint` | `const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint` | IP address plus host-order port; input or output according to pointer constness. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

If unopened, it creates a UDP socket with the peer address family. Connected UDP filters incoming datagrams to that peer and permits Send/Receive without an explicit destination; this is not a TCP handshake. Sets `hasPeer` on success.

---

### Example

```c
OPSTATUS status = NETWORK_DATAGRAM_FUNC(SetPeer)(
    &datagram, &endpoint, &error);
```

---

# Datagram SendTo

Sends a datagram to an explicitly named endpoint.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_DATAGRAM_FUNC(SendTo)( NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint, const void *data, size_t size, size_t *sent, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Datagram_SendTo( Network_Datagram_TDatagram *datagram, const Network_Address_TEndpoint *endpoint, const void *data, size_t size, size_t *sent, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `datagram` | `NETWORK_DATAGRAM_TYPE(TDatagram) *datagram` | Initialized UDP datagram object. |
| `endpoint` | `const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint` | IP address plus host-order port; input or output according to pointer constness. |
| `data` | `const void *data` | Bytes to transmit. |
| `size` | `size_t size` | Byte count to send, read, or transfer. |
| `sent` | `size_t *sent` | Writable number of bytes sent; see optionality for SendTo. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Can auto-open UDP socket if unopened. Requires a non-NULL sent count and endpoint. Destination address family must match an already-open socket.

---

### Example

```c
const char msg[] = "udp"; size_t sent = 0;
OPSTATUS status = NETWORK_DATAGRAM_FUNC(SendTo)(&datagram,
    &endpoint, msg, sizeof(msg), &sent, &error);
```

---

# Datagram ReceiveFrom

Receives a datagram with optional source endpoint.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_DATAGRAM_FUNC(ReceiveFrom)( NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, void *buffer, size_t capacity, NETWORK_ADDRESS_TYPE(TEndpoint) *sender, size_t *received, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Datagram_ReceiveFrom( Network_Datagram_TDatagram *datagram, void *buffer, size_t capacity, Network_Address_TEndpoint *sender, size_t *received, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `datagram` | `NETWORK_DATAGRAM_TYPE(TDatagram) *datagram` | Initialized UDP datagram object. |
| `buffer` | `void *buffer` | Caller-owned writable byte buffer. |
| `capacity` | `size_t capacity` | Available capacity of destination buffer, in bytes. |
| `sender` | `NETWORK_ADDRESS_TYPE(TEndpoint) *sender` | Optional output UDP sender endpoint; may be NULL. |
| `received` | `size_t *received` | Writable number of bytes received. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Requires an open UDP socket and a `received` pointer. `sender` may be NULL. A successful UDP message can be zero length. Does not imply the datagram has a connected peer.

---

### Example

```c
char packet[512]; size_t received = 0;
NETWORK_ADDRESS_TYPE(TEndpoint) sender;
OPSTATUS status = NETWORK_DATAGRAM_FUNC(ReceiveFrom)(&datagram,
    packet, sizeof(packet), &sender, &received, &error);
```

---

# Datagram Send

Sends through the associated UDP peer.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_DATAGRAM_FUNC(Send)( NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, const void *data, size_t size, size_t *sent, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Datagram_Send( Network_Datagram_TDatagram *datagram, const void *data, size_t size, size_t *sent, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `datagram` | `NETWORK_DATAGRAM_TYPE(TDatagram) *datagram` | Initialized UDP datagram object. |
| `data` | `const void *data` | Bytes to transmit. |
| `size` | `size_t size` | Byte count to send, read, or transfer. |
| `sent` | `size_t *sent` | Writable number of bytes sent; see optionality for SendTo. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Requires a successful SetPeer first. This sends a datagram, not a stream; `sent` is required and the packet must fit platform limits.

---

### Example

```c
const char packet[] = "hello"; size_t sent = 0;
OPSTATUS status = NETWORK_DATAGRAM_FUNC(Send)(&datagram,
    packet, sizeof(packet), &sent, &error);
```

---

# Datagram Receive

Reads datagrams from the associated UDP peer.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_DATAGRAM_FUNC(Receive)( NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, void *buffer, size_t capacity, size_t *received, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Datagram_Receive( Network_Datagram_TDatagram *datagram, void *buffer, size_t capacity, size_t *received, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `datagram` | `NETWORK_DATAGRAM_TYPE(TDatagram) *datagram` | Initialized UDP datagram object. |
| `buffer` | `void *buffer` | Caller-owned writable byte buffer. |
| `capacity` | `size_t capacity` | Available capacity of destination buffer, in bytes. |
| `received` | `size_t *received` | Writable number of bytes received. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Requires hasPeer=true. Delegates to low-level Socket_Receive; because that primitive interprets native zero reads as a closed connection, zero-length connected datagrams may not be reported like ReceiveFrom.

---

### Example

```c
char packet[512]; size_t n = 0;
OPSTATUS status = NETWORK_DATAGRAM_FUNC(Receive)(&datagram,
    packet, sizeof(packet), &n, &error);
```

---

# Datagram SetBlocking

Sets the UDP socket's native blocking mode.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_DATAGRAM_FUNC(SetBlocking)( NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, bool blocking, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Datagram_SetBlocking( Network_Datagram_TDatagram *datagram, bool blocking, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `datagram` | `NETWORK_DATAGRAM_TYPE(TDatagram) *datagram` | Initialized UDP datagram object. |
| `blocking` | `bool blocking` | True for blocking operations, false for nonblocking. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Requires the socket to have been opened; changes both the native socket mode and stored preference upon success.

---

### Example

```c
OPSTATUS status = NETWORK_DATAGRAM_FUNC(SetBlocking)(&datagram, false, &error);
```

---

# Datagram ClearPeer

Clears the previously configured UDP peer association.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_DATAGRAM_FUNC(ClearPeer)( NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Datagram_ClearPeer( Network_Datagram_TDatagram *datagram, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `datagram` | `NETWORK_DATAGRAM_TYPE(TDatagram) *datagram` | Initialized UDP datagram object. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Attempts native AF_UNSPEC disconnect. On Windows/macOS, specified native errors are tolerated; on other POSIX targets a failure can produce GENERIC_ERROR. **Logical `hasPeer` and cached endpoint are cleared even when native disconnect reports an error**; check return status.

---

### Example

```c
OPSTATUS status = NETWORK_DATAGRAM_FUNC(ClearPeer)(&datagram, &error);
```

---

# Datagram Close

Closes UDP socket and forgets that a peer was configured.

### Syntax

#### Macro form

```c
void NETWORK_DATAGRAM_FUNC(Close)( NETWORK_DATAGRAM_TYPE(TDatagram) *datagram);
```

#### Direct form

```c
void Network_Datagram_Close( Network_Datagram_TDatagram *datagram);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `datagram` | `NETWORK_DATAGRAM_TYPE(TDatagram) *datagram` | Initialized UDP datagram object. |

---

### Return value

None.

---

### Remarks

Invalidates socket handle and clears hasPeer, but does not reset all endpoint fields or automatically free user state. NULL is ignored.

---

### Example

```c
NETWORK_DATAGRAM_FUNC(Close)(&datagram);
```

---

# Datagram HasPeer

Tests whether a UDP peer is configured.

### Syntax

#### Macro form

```c
bool NETWORK_DATAGRAM_FUNC(HasPeer)( const NETWORK_DATAGRAM_TYPE(TDatagram) *datagram);
```

#### Direct form

```c
bool Network_Datagram_HasPeer( const Network_Datagram_TDatagram *datagram);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `datagram` | `const NETWORK_DATAGRAM_TYPE(TDatagram) *datagram` | Initialized UDP datagram object. |

---

### Return value

`bool`: true when the tested property holds, false otherwise.

---

### Remarks

Returns false for NULL, or the value of `datagram->hasPeer`. Does not perform a network check.

---

### Example

```c
bool configured = NETWORK_DATAGRAM_FUNC(HasPeer)(&datagram);
```

---

# Datagram LocalEndpoint

Reports the cached local UDP endpoint.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_DATAGRAM_FUNC(LocalEndpoint)( const NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, NETWORK_ADDRESS_TYPE(TEndpoint) *outEndpoint);
```

#### Direct form

```c
OPSTATUS Network_Datagram_LocalEndpoint( const Network_Datagram_TDatagram *datagram, Network_Address_TEndpoint *outEndpoint);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `datagram` | `const NETWORK_DATAGRAM_TYPE(TDatagram) *datagram` | Initialized UDP datagram object. |
| `outEndpoint` | `NETWORK_ADDRESS_TYPE(TEndpoint) *outEndpoint` | Writable endpoint output. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Requires an open native socket; returns NOT_AVAILABLE if it is not open. Usually refreshed after Bind, SetPeer or SendTo; a plain Open may leave cached endpoint zeroed.

---

### Example

```c
NETWORK_ADDRESS_TYPE(TEndpoint) local;
OPSTATUS status = NETWORK_DATAGRAM_FUNC(LocalEndpoint)(&datagram, &local);
```

---

# Datagram PeerEndpoint

Reports the configured UDP peer endpoint.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_DATAGRAM_FUNC(PeerEndpoint)( const NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, NETWORK_ADDRESS_TYPE(TEndpoint) *outEndpoint);
```

#### Direct form

```c
OPSTATUS Network_Datagram_PeerEndpoint( const Network_Datagram_TDatagram *datagram, Network_Address_TEndpoint *outEndpoint);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `datagram` | `const NETWORK_DATAGRAM_TYPE(TDatagram) *datagram` | Initialized UDP datagram object. |
| `outEndpoint` | `NETWORK_ADDRESS_TYPE(TEndpoint) *outEndpoint` | Writable endpoint output. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Requires hasPeer=true; otherwise returns NOT_FOUND. Copies the endpoint into the output parameter.

---

### Example

```c
NETWORK_ADDRESS_TYPE(TEndpoint) remote;
OPSTATUS status = NETWORK_DATAGRAM_FUNC(PeerEndpoint)(&datagram, &remote);
```

---

# Listener package

Header: `Cosmeron/Modules/Network/Listener/Listener.h`

`TListener` may manage one or two native listening sockets. `Listen` chooses available IPv6/IPv4 coverage; `ListenAt` binds an explicitly requested endpoint. Accepted clients become independently owned TConnection objects.

### Types

| Type / constant | Contents |
| --- | --- |
| `NETWORK_LISTENER_TYPE(TListener)` | Owned `.primary` and `.secondary` Socket values, `.localEndpoint`, `.blocking`, `.dualStack` |
| `NETWORK_LISTENER_DEFAULT_BACKLOG` | `128` |

### Function summary

| Function | Description |
| --- | --- |
| [`Init`](#listener-init) | Initializes a listening TCP server object. |
| [`Listen`](#listener-listen) | Listens on a port, preferring IPv6 with IPv4 fallback. |
| [`ListenAt`](#listener-listenat) | Listens on an explicit local IP endpoint. |
| [`Accept`](#listener-accept) | Accepts an incoming TCP client as a high-level connection. |
| [`Close`](#listener-close) | Closes the listener's primary and secondary sockets. |
| [`LocalEndpoint`](#listener-localendpoint) | Returns the primary listener's bound endpoint. |

The following examples may reference a previously initialized object, such as `socket`, `connection`, `datagram`, `listener`, `poller`, `endpoint` or `error`. Instantiate and validate the appropriate state before using a fragment.

---

# Listener Init

Initializes a listening TCP server object.

### Syntax

#### Macro form

```c
void NETWORK_LISTENER_FUNC(Init)( NETWORK_LISTENER_TYPE(TListener) *listener);
```

#### Direct form

```c
void Network_Listener_Init( Network_Listener_TListener *listener);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `listener` | `NETWORK_LISTENER_TYPE(TListener) *listener` | Initialized listening server object. |

---

### Return value

None.

---

### Remarks

Sets blocking=true and invalidates both primary/secondary native sockets. Must precede Listen/ListenAt/Close.

---

### Example

```c
NETWORK_LISTENER_TYPE(TListener) listener;
NETWORK_LISTENER_FUNC(Init)(&listener);
```

---

# Listener Listen

Listens on a port, preferring IPv6 with IPv4 fallback.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_LISTENER_FUNC(Listen)( NETWORK_LISTENER_TYPE(TListener) *listener, uint16_t port, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Listener_Listen( Network_Listener_TListener *listener, uint16_t port, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `listener` | `NETWORK_LISTENER_TYPE(TListener) *listener` | Initialized listening server object. |
| `port` | `uint16_t port` | Port number in host byte order. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Tries an IPv6 wildcard listener. If native socket is dual-stack, one handle serves both; otherwise it attempts an IPv4 companion socket on the same bound port. If IPv6 works but the IPv4 companion fails, IPv6 service remains valid. Falls back to IPv4 when IPv6 cannot open. Backlog default is 128.

---

### Example

```c
OPSTATUS status = NETWORK_LISTENER_FUNC(Listen)(&listener, 8080, &error);
```

---

# Listener ListenAt

Listens on an explicit local IP endpoint.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_LISTENER_FUNC(ListenAt)( NETWORK_LISTENER_TYPE(TListener) *listener, const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Listener_ListenAt( Network_Listener_TListener *listener, const Network_Address_TEndpoint *endpoint, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `listener` | `NETWORK_LISTENER_TYPE(TListener) *listener` | Initialized listening server object. |
| `endpoint` | `const NETWORK_ADDRESS_TYPE(TEndpoint) *endpoint` | IP address plus host-order port; input or output according to pointer constness. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Opens only the requested family/address (unlike Listen's dual-stack strategy), using SO_REUSEADDR and backlog 128. Closes any previous listener before re-listening.

---

### Example

```c
OPSTATUS status = NETWORK_LISTENER_FUNC(ListenAt)(&listener,
    &endpoint, &error);
```

---

# Listener Accept

Accepts an incoming TCP client as a high-level connection.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_LISTENER_FUNC(Accept)( NETWORK_LISTENER_TYPE(TListener) *listener, NETWORK_CONNECTION_TYPE(TConnection) *connection, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Listener_Accept( Network_Listener_TListener *listener, Network_Connection_TConnection *connection, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `listener` | `NETWORK_LISTENER_TYPE(TListener) *listener` | Initialized listening server object. |
| `connection` | `NETWORK_CONNECTION_TYPE(TConnection) *connection` | High-level TCP connection object. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Writes a newly initialized, CONNECTED connection to the output **without inspecting its previous contents**. Do not pass an already-owning live connection or resources may be lost. Blocking behavior depends on native listener sockets; this call may wait for a peer.

---

### Example

```c
NETWORK_CONNECTION_TYPE(TConnection) accepted;
OPSTATUS status = NETWORK_LISTENER_FUNC(Accept)(&listener,
    &accepted, &error);
if (status == STATUS_CONST(SUCCESS))
    NETWORK_CONNECTION_FUNC(Destroy)(&accepted);
```

---

# Listener Close

Closes the listener's primary and secondary sockets.

### Syntax

#### Macro form

```c
void NETWORK_LISTENER_FUNC(Close)( NETWORK_LISTENER_TYPE(TListener) *listener);
```

#### Direct form

```c
void Network_Listener_Close( Network_Listener_TListener *listener);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `listener` | `NETWORK_LISTENER_TYPE(TListener) *listener` | Initialized listening server object. |

---

### Return value

None.

---

### Remarks

Clears dualStack. Does not close connections that were already accepted. NULL is ignored.

---

### Example

```c
NETWORK_LISTENER_FUNC(Close)(&listener);
```

---

# Listener LocalEndpoint

Returns the primary listener's bound endpoint.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_LISTENER_FUNC(LocalEndpoint)( const NETWORK_LISTENER_TYPE(TListener) *listener, NETWORK_ADDRESS_TYPE(TEndpoint) *outEndpoint);
```

#### Direct form

```c
OPSTATUS Network_Listener_LocalEndpoint( const Network_Listener_TListener *listener, Network_Address_TEndpoint *outEndpoint);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `listener` | `const NETWORK_LISTENER_TYPE(TListener) *listener` | Initialized listening server object. |
| `outEndpoint` | `NETWORK_ADDRESS_TYPE(TEndpoint) *outEndpoint` | Writable endpoint output. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Requires an open primary socket. If Listen used port zero, this reports the OS-selected port. A multi-socket listener may have a secondary IPv4 socket with the same port.

---

### Example

```c
NETWORK_ADDRESS_TYPE(TEndpoint) local;
OPSTATUS status = NETWORK_LISTENER_FUNC(LocalEndpoint)(&listener, &local);
```

---

# Poller package

Header: `Cosmeron/Modules/Network/Poll/Poll.h`

`TPoller` stores registrations and does not own their objects. It produces an owning `TEventVector` (Container Flat Vector) that must be initialized and destroyed by the caller. Poll is synchronous and platform-specific.

### Types

| Type / constants | Contents |
| --- | --- |
| `NETWORK_POLLER_TYPE(TPoller)` | Owning registration vector, `nextHandle` |
| `NETWORK_POLLER_TYPE(Handle)` | `uint64_t` registration identifier, nonzero |
| `NETWORK_POLLER_TYPE(EventFlags)` | `uint32_t` readiness mask |
| `NETWORK_POLLER_TYPE(TRegistration)` | Handle, source type, interests, borrowed object/userdata |
| `NETWORK_POLLER_TYPE(TEvent)` | Handle, source type, flags, borrowed object/userdata |
| `NETWORK_POLLER_TYPE(TEventVector)` | Flat Vector of TEvent |
| `NETWORK_POLLER_TYPE(TSourceType)` | `NETWORK_POLLER_SOURCE_CONST(CONNECTION/LISTENER/DATAGRAM)` |

### Function summary

| Function | Description |
| --- | --- |
| [`Init`](#poller-init) | Initializes an empty poller and registration vector. |
| [`Destroy`](#poller-destroy) | Releases poller registration storage. |
| [`AddConnection`](#poller-addconnection) | Registers a connection for selected readiness interests. |
| [`AddListener`](#poller-addlistener) | Registers a listener for readiness/accept events. |
| [`AddDatagram`](#poller-adddatagram) | Registers a UDP datagram object. |
| [`Remove`](#poller-remove) | Unregisters a previously returned registration handle. |
| [`Watch`](#poller-watch) | Changes interests for an existing handle. |
| [`Wait`](#poller-wait) | Waits for readiness with no time limit. |
| [`WaitFor`](#poller-waitfor) | Waits for readiness for at most the requested duration. |
| [`Event_Has`](#poller-event_has) | Tests whether an event contains all requested flag bits. |
| [`Event_UserData`](#poller-event_userdata) | Retrieves the user pointer captured at registration time. |
| [`Event_Connection`](#poller-event_connection) | Retrieves the connection associated with a connection event. |
| [`Event_Listener`](#poller-event_listener) | Retrieves the listener associated with a listener event. |
| [`Event_Datagram`](#poller-event_datagram) | Retrieves the datagram object associated with a UDP event. |

The following examples may reference a previously initialized object, such as `socket`, `connection`, `datagram`, `listener`, `poller`, `endpoint` or `error`. Instantiate and validate the appropriate state before using a fragment.

---

# Poller Init

Initializes an empty poller and registration vector.

### Syntax

#### Macro form

```c
void NETWORK_POLLER_FUNC(Init)(NETWORK_POLLER_TYPE(TPoller) *poller);
```

#### Direct form

```c
void Network_Poller_Init(Network_Poller_TPoller *poller);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `poller` | `NETWORK_POLLER_TYPE(TPoller) *poller` | Initialized poller managing borrowed registrations. |

---

### Return value

None.

---

### Remarks

Sets nextHandle=1. Call before Add*/Wait. This is a synchronous readiness poller, not a worker thread.

---

### Example

```c
NETWORK_POLLER_TYPE(TPoller) poller;
NETWORK_POLLER_FUNC(Init)(&poller);
```

---

# Poller Destroy

Releases poller registration storage.

### Syntax

#### Macro form

```c
void NETWORK_POLLER_FUNC(Destroy)(NETWORK_POLLER_TYPE(TPoller) *poller);
```

#### Direct form

```c
void Network_Poller_Destroy(Network_Poller_TPoller *poller);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `poller` | `NETWORK_POLLER_TYPE(TPoller) *poller` | Initialized poller managing borrowed registrations. |

---

### Return value

None.

---

### Remarks

Does **not** close or destroy the registered connection/listener/datagram objects. Previously returned registration handles become invalid; NULL ignored.

---

### Example

```c
NETWORK_POLLER_FUNC(Destroy)(&poller);
```

---

# Poller AddConnection

Registers a connection for selected readiness interests.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_POLLER_FUNC(AddConnection)( NETWORK_POLLER_TYPE(TPoller) *poller, NETWORK_CONNECTION_TYPE(TConnection) *connection, NETWORK_POLLER_TYPE(EventFlags) interests, void *userdata, NETWORK_POLLER_TYPE(Handle) *handle, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Poller_AddConnection( Network_Poller_TPoller *poller, Network_Connection_TConnection *connection, Network_Poller_EventFlags interests, void *userdata, Network_Poller_Handle *handle, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `poller` | `NETWORK_POLLER_TYPE(TPoller) *poller` | Initialized poller managing borrowed registrations. |
| `connection` | `NETWORK_CONNECTION_TYPE(TConnection) *connection` | High-level TCP connection object. |
| `interests` | `NETWORK_POLLER_TYPE(EventFlags) interests` | Bit mask of readiness events to watch. |
| `userdata` | `void *userdata` | Optional pointer passed through to resulting events. |
| `handle` | `NETWORK_POLLER_TYPE(Handle) *handle` | Output registration handle; receives a nonzero ID. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Stores a borrowed pointer to the live TConnection, a flags mask and optional userdata. Outputs a nonzero registration Handle, not a socket descriptor. Keep the connection at a stable address until removed/destroyed.

---

### Example

```c
NETWORK_POLLER_TYPE(Handle) id = 0;
OPSTATUS status = NETWORK_POLLER_FUNC(AddConnection)(&poller,
    &connection, NETWORK_EVENT_CONST(READ) | NETWORK_EVENT_CONST(CLOSE),
    NULL, &id, &error);
```

---

# Poller AddListener

Registers a listener for readiness/accept events.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_POLLER_FUNC(AddListener)( NETWORK_POLLER_TYPE(TPoller) *poller, NETWORK_LISTENER_TYPE(TListener) *listener, NETWORK_POLLER_TYPE(EventFlags) interests, void *userdata, NETWORK_POLLER_TYPE(Handle) *handle, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Poller_AddListener( Network_Poller_TPoller *poller, Network_Listener_TListener *listener, Network_Poller_EventFlags interests, void *userdata, Network_Poller_Handle *handle, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `poller` | `NETWORK_POLLER_TYPE(TPoller) *poller` | Initialized poller managing borrowed registrations. |
| `listener` | `NETWORK_LISTENER_TYPE(TListener) *listener` | Initialized listening server object. |
| `interests` | `NETWORK_POLLER_TYPE(EventFlags) interests` | Bit mask of readiness events to watch. |
| `userdata` | `void *userdata` | Optional pointer passed through to resulting events. |
| `handle` | `NETWORK_POLLER_TYPE(Handle) *handle` | Output registration handle; receives a nonzero ID. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Stores borrowed listener pointer and userdata. On dual-socket listeners, readiness of both primary/secondary handles is combined into one TEvent for the registration.

---

### Example

```c
NETWORK_POLLER_TYPE(Handle) id = 0;
OPSTATUS status = NETWORK_POLLER_FUNC(AddListener)(&poller,
    &listener, NETWORK_EVENT_CONST(ACCEPT), NULL, &id, &error);
```

---

# Poller AddDatagram

Registers a UDP datagram object.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_POLLER_FUNC(AddDatagram)( NETWORK_POLLER_TYPE(TPoller) *poller, NETWORK_DATAGRAM_TYPE(TDatagram) *datagram, NETWORK_POLLER_TYPE(EventFlags) interests, void *userdata, NETWORK_POLLER_TYPE(Handle) *handle, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Poller_AddDatagram( Network_Poller_TPoller *poller, Network_Datagram_TDatagram *datagram, Network_Poller_EventFlags interests, void *userdata, Network_Poller_Handle *handle, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `poller` | `NETWORK_POLLER_TYPE(TPoller) *poller` | Initialized poller managing borrowed registrations. |
| `datagram` | `NETWORK_DATAGRAM_TYPE(TDatagram) *datagram` | Initialized UDP datagram object. |
| `interests` | `NETWORK_POLLER_TYPE(EventFlags) interests` | Bit mask of readiness events to watch. |
| `userdata` | `void *userdata` | Optional pointer passed through to resulting events. |
| `handle` | `NETWORK_POLLER_TYPE(Handle) *handle` | Output registration handle; receives a nonzero ID. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Stores borrowed UDP object address, desired flags and userdata; the registration does not open the UDP socket itself.

---

### Example

```c
NETWORK_POLLER_TYPE(Handle) id = 0;
OPSTATUS status = NETWORK_POLLER_FUNC(AddDatagram)(&poller,
    &datagram, NETWORK_EVENT_CONST(READ), NULL, &id, &error);
```

---

# Poller Remove

Unregisters a previously returned registration handle.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_POLLER_FUNC(Remove)( NETWORK_POLLER_TYPE(TPoller) *poller, NETWORK_POLLER_TYPE(Handle) handle, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Poller_Remove( Network_Poller_TPoller *poller, Network_Poller_Handle handle, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `poller` | `NETWORK_POLLER_TYPE(TPoller) *poller` | Initialized poller managing borrowed registrations. |
| `handle` | `NETWORK_POLLER_TYPE(Handle) handle` | Registration handle, input or output depending on signature (not a native descriptor). |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

`handle == 0` is invalid; unknown nonzero handles return NOT_FOUND. Does not close or destroy the referenced object.

---

### Example

```c
OPSTATUS status = NETWORK_POLLER_FUNC(Remove)(&poller, id, &error);
```

---

# Poller Watch

Changes interests for an existing handle.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_POLLER_FUNC(Watch)( NETWORK_POLLER_TYPE(TPoller) *poller, NETWORK_POLLER_TYPE(Handle) handle, NETWORK_POLLER_TYPE(EventFlags) interests, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Poller_Watch( Network_Poller_TPoller *poller, Network_Poller_Handle handle, Network_Poller_EventFlags interests, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `poller` | `NETWORK_POLLER_TYPE(TPoller) *poller` | Initialized poller managing borrowed registrations. |
| `handle` | `NETWORK_POLLER_TYPE(Handle) handle` | Registration handle, input or output depending on signature (not a native descriptor). |
| `interests` | `NETWORK_POLLER_TYPE(EventFlags) interests` | Bit mask of readiness events to watch. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Replaces (does not append to) its event mask; unknown handles return NOT_FOUND. Registrations retain their userdata and borrowed object pointer.

---

### Example

```c
OPSTATUS status = NETWORK_POLLER_FUNC(Watch)(&poller, id,
    NETWORK_EVENT_CONST(READ) | NETWORK_EVENT_CONST(WRITE), &error);
```

---

# Poller Wait

Waits for readiness with no time limit.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_POLLER_FUNC(Wait)( NETWORK_POLLER_TYPE(TPoller) *poller, NETWORK_POLLER_TYPE(TEventVector) *events, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Poller_Wait( Network_Poller_TPoller *poller, Network_Poller_TEventVector *events, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `poller` | `NETWORK_POLLER_TYPE(TPoller) *poller` | Initialized poller managing borrowed registrations. |
| `events` | `NETWORK_POLLER_TYPE(TEventVector) *events` | Owning vector of poll events; initialize before Wait/WaitFor. |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Writes an owning TEventVector, replacing/destroying its prior contents on success. **Initialize events first** with `FLAT_VECTOR_FUNC(Network_Event, Init)`, and destroy when finished. With no registrations or valid native handles, returns immediately with an empty vector instead of blocking.

---

### Example

```c
OPSTATUS status = NETWORK_POLLER_FUNC(Wait)(&poller, &events, &error);
```

---

# Poller WaitFor

Waits for readiness for at most the requested duration.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_POLLER_FUNC(WaitFor)( NETWORK_POLLER_TYPE(TPoller) *poller, NETWORK_POLLER_TYPE(TEventVector) *events, CHRONOMETRY_TYPE(Duration) timeout, NETWORK_TYPE(TError) *error);
```

#### Direct form

```c
OPSTATUS Network_Poller_WaitFor( Network_Poller_TPoller *poller, Network_Poller_TEventVector *events, Chronometry_Duration timeout, Network_TError *error);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `poller` | `NETWORK_POLLER_TYPE(TPoller) *poller` | Initialized poller managing borrowed registrations. |
| `events` | `NETWORK_POLLER_TYPE(TEventVector) *events` | Owning vector of poll events; initialize before Wait/WaitFor. |
| `timeout` | `CHRONOMETRY_TYPE(Duration) timeout` | Nonnegative Chronometry Duration (nanoseconds). |
| `error` | `NETWORK_TYPE(TError) *error` | Optional output network error; may be NULL. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Timeout is a nonnegative Chronometry Duration; zero requests a nonblocking poll. Platform rounds/clamps duration to select/WSAPoll limits. **Timeout is not an error**: returns SUCCESS with an empty vector. Replaces event vector on success; may return OUT_OF_RANGE on POSIX FD_SETSIZE overflow.

---

### Example

```c
CHRONOMETRY_TYPE(Duration) timeout = DURATION_FUNC(FromNanoseconds)(100000000);
OPSTATUS status = NETWORK_POLLER_FUNC(WaitFor)(&poller,
    &events, timeout, &error);
```

---

# Poller Event_Has

Tests whether an event contains all requested flag bits.

### Syntax

#### Macro form

```c
bool NETWORK_POLLER_FUNC(Event_Has)( const NETWORK_POLLER_TYPE(TEvent) *event, NETWORK_POLLER_TYPE(EventFlags) flags);
```

#### Direct form

```c
bool Network_Poller_Event_Has( const Network_Poller_TEvent *event, Network_Poller_EventFlags flags);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `event` | `const NETWORK_POLLER_TYPE(TEvent) *event` | Pointer to one event. |
| `flags` | `NETWORK_POLLER_TYPE(EventFlags) flags` | Readiness flag mask to test. |

---

### Return value

`bool`: true when the tested property holds, false otherwise.

---

### Remarks

Evaluates `(event->flags & flags) == flags`; NULL event returns false. A zero flags mask matches any non-NULL event. This is an all-bits test, not an any-bits test.

---

### Example

```c
bool ready = NETWORK_POLLER_FUNC(Event_Has)(
    &events.data[0], NETWORK_EVENT_CONST(READ));
```

---

# Poller Event_UserData

Retrieves the user pointer captured at registration time.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_POLLER_FUNC(Event_UserData)( const NETWORK_POLLER_TYPE(TEvent) *event, void **outUserdata);
```

#### Direct form

```c
OPSTATUS Network_Poller_Event_UserData( const Network_Poller_TEvent *event, void **outUserdata);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `event` | `const NETWORK_POLLER_TYPE(TEvent) *event` | Pointer to one event. |
| `outUserdata` | `void **outUserdata` | Writable `void *` output; the result is borrowed. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

The returned pointer may be NULL by design. This is borrowed memory; poller does not manage its lifetime.

---

### Example

```c
void *context = NULL;
OPSTATUS status = NETWORK_POLLER_FUNC(Event_UserData)(
    &events.data[0], &context);
```

---

# Poller Event_Connection

Retrieves the connection associated with a connection event.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_POLLER_FUNC(Event_Connection)( const NETWORK_POLLER_TYPE(TEvent) *event, NETWORK_CONNECTION_TYPE(TConnection) **outConnection);
```

#### Direct form

```c
OPSTATUS Network_Poller_Event_Connection( const Network_Poller_TEvent *event, Network_Connection_TConnection **outConnection);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `event` | `const NETWORK_POLLER_TYPE(TEvent) *event` | Pointer to one event. |
| `outConnection` | `NETWORK_CONNECTION_TYPE(TConnection) **outConnection` | Writable borrowed connection pointer. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Requires event.sourceType == POLLER_SOURCE_CONNECTION; otherwise returns NOT_FOUND. Writes a borrowed pointer: do not free or destroy through the event object unless you own that connection.

---

### Example

```c
NETWORK_CONNECTION_TYPE(TConnection) *found = NULL;
OPSTATUS status = NETWORK_POLLER_FUNC(Event_Connection)(
    &events.data[0], &found);
```

---

# Poller Event_Listener

Retrieves the listener associated with a listener event.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_POLLER_FUNC(Event_Listener)( const NETWORK_POLLER_TYPE(TEvent) *event, NETWORK_LISTENER_TYPE(TListener) **outListener);
```

#### Direct form

```c
OPSTATUS Network_Poller_Event_Listener( const Network_Poller_TEvent *event, Network_Listener_TListener **outListener);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `event` | `const NETWORK_POLLER_TYPE(TEvent) *event` | Pointer to one event. |
| `outListener` | `NETWORK_LISTENER_TYPE(TListener) **outListener` | Writable borrowed listener pointer. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Requires matching POLLER_SOURCE_LISTENER; mismatch returns NOT_FOUND. Output pointer is borrowed.

---

### Example

```c
NETWORK_LISTENER_TYPE(TListener) *found = NULL;
OPSTATUS status = NETWORK_POLLER_FUNC(Event_Listener)(
    &events.data[0], &found);
```

---

# Poller Event_Datagram

Retrieves the datagram object associated with a UDP event.

### Syntax

#### Macro form

```c
OPSTATUS NETWORK_POLLER_FUNC(Event_Datagram)( const NETWORK_POLLER_TYPE(TEvent) *event, NETWORK_DATAGRAM_TYPE(TDatagram) **outDatagram);
```

#### Direct form

```c
OPSTATUS Network_Poller_Event_Datagram( const Network_Poller_TEvent *event, Network_Datagram_TDatagram **outDatagram);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `event` | `const NETWORK_POLLER_TYPE(TEvent) *event` | Pointer to one event. |
| `outDatagram` | `NETWORK_DATAGRAM_TYPE(TDatagram) **outDatagram` | Writable borrowed datagram pointer. |

---

### Return value

`OPSTATUS`: returns `STATUS_CONST(SUCCESS)` on success or a recoverable status (often `INVALID_ARGUMENT`, `NOT_FOUND`, `NOT_AVAILABLE`, `OUT_OF_MEMORY`, or `GENERIC_ERROR`). Network-level details may be reported via the optional `error` pointer.

---

### Remarks

Requires matching POLLER_SOURCE_DATAGRAM; mismatch returns NOT_FOUND. Output pointer is borrowed.

---

### Example

```c
NETWORK_DATAGRAM_TYPE(TDatagram) *found = NULL;
OPSTATUS status = NETWORK_POLLER_FUNC(Event_Datagram)(
    &events.data[0], &found);
```

---

# Complete examples

## TCP: loopback client and server

A local echo-style exchange without external network connectivity. The listener binds loopback and asks the OS for an ephemeral port; accepted connections are independently destroyed.

```c
#include <string.h>
#include "Cosmeron/Modules/Network/Listener/Listener.h"

int main(void) {
    NETWORK_LISTENER_TYPE(TListener) listener;
    NETWORK_CONNECTION_TYPE(TConnection) client, accepted;
    NETWORK_ADDRESS_TYPE(TAddress) address;
    NETWORK_ADDRESS_TYPE(TEndpoint) endpoint;
    NETWORK_TYPE(TError) networkError = NETWORK_ERROR_CONST(NONE);
    char received[5] = {0};
    const char message[5] = {'h', 'e', 'l', 'l', 'o'};
    OPSTATUS status;

    NETWORK_LISTENER_FUNC(Init)(&listener);
    NETWORK_CONNECTION_FUNC(Init)(&client);
    NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&address);
    NETWORK_ADDRESS_FUNC(Endpoint_Create)(&endpoint, &address, 0);

    status = NETWORK_LISTENER_FUNC(ListenAt)(&listener, &endpoint, &networkError);
    if (status != STATUS_CONST(SUCCESS)) goto cleanup;

    status = NETWORK_LISTENER_FUNC(LocalEndpoint)(&listener, &endpoint);
    if (status != STATUS_CONST(SUCCESS)) goto cleanup;
    status = NETWORK_CONNECTION_FUNC(Connect)(
        &client, "127.0.0.1", endpoint.port, &networkError);
    if (status != STATUS_CONST(SUCCESS)) goto cleanup;

    status = NETWORK_LISTENER_FUNC(Accept)(&listener, &accepted, &networkError);
    if (status != STATUS_CONST(SUCCESS)) goto cleanup;
    status = NETWORK_CONNECTION_FUNC(WriteAll)(
        &client, message, sizeof(message), &networkError);
    if (status == STATUS_CONST(SUCCESS))
        status = NETWORK_CONNECTION_FUNC(ReadExact)(
            &accepted, received, sizeof(received), &networkError);

    NETWORK_CONNECTION_FUNC(Destroy)(&accepted);
    if (status == STATUS_CONST(SUCCESS) &&
        memcmp(message, received, sizeof(message)) != 0)
        status = STATUS_CONST(GENERIC_ERROR);

cleanup:
    NETWORK_CONNECTION_FUNC(Destroy)(&client);
    NETWORK_LISTENER_FUNC(Close)(&listener);
    return status == STATUS_CONST(SUCCESS) ? 0 : 1;
}
```

## UDP: send to a bound loopback socket

```c
#include <string.h>
#include "Cosmeron/Modules/Network/Datagram/Datagram.h"

int main(void) {
    NETWORK_DATAGRAM_TYPE(TDatagram) sender, receiver;
    NETWORK_ADDRESS_TYPE(TAddress) address;
    NETWORK_ADDRESS_TYPE(TEndpoint) local, destination, peer;
    const char payload[] = "udp";
    char packet[32] = {0};
    size_t sent = 0, received = 0;
    OPSTATUS status;

    NETWORK_DATAGRAM_FUNC(Init)(&sender);
    NETWORK_DATAGRAM_FUNC(Init)(&receiver);
    NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&address);
    NETWORK_ADDRESS_FUNC(Endpoint_Create)(&local, &address, 0);

    status = NETWORK_DATAGRAM_FUNC(Bind)(&receiver, &local, NULL);
    if (status == STATUS_CONST(SUCCESS))
        status = NETWORK_DATAGRAM_FUNC(LocalEndpoint)(&receiver, &destination);
    if (status == STATUS_CONST(SUCCESS))
        status = NETWORK_DATAGRAM_FUNC(SendTo)(
            &sender, &destination, payload, sizeof(payload), &sent, NULL);
    if (status == STATUS_CONST(SUCCESS))
        status = NETWORK_DATAGRAM_FUNC(ReceiveFrom)(
            &receiver, packet, sizeof(packet), &peer, &received, NULL);
    int valid = status == STATUS_CONST(SUCCESS) &&
                received == sizeof(payload) &&
                memcmp(packet, payload, sizeof(payload)) == 0;

    NETWORK_DATAGRAM_FUNC(Close)(&sender);
    NETWORK_DATAGRAM_FUNC(Close)(&receiver);
    return valid ? 0 : 1;
}
```

## Poller: listen for an incoming TCP connection

```c
#include "Cosmeron/Modules/Network/Poll/Poll.h"

int main(void) {
    NETWORK_LISTENER_TYPE(TListener) listener;
    NETWORK_CONNECTION_TYPE(TConnection) client;
    NETWORK_POLLER_TYPE(TPoller) poller;
    NETWORK_POLLER_TYPE(TEventVector) events;
    NETWORK_POLLER_TYPE(Handle) registration = 0;
    NETWORK_ADDRESS_TYPE(TAddress) address;
    NETWORK_ADDRESS_TYPE(TEndpoint) endpoint;
    CHRONOMETRY_TYPE(Duration) timeout =
        DURATION_FUNC(FromNanoseconds)(1000000000);
    OPSTATUS status;

    NETWORK_LISTENER_FUNC(Init)(&listener);
    NETWORK_CONNECTION_FUNC(Init)(&client);
    NETWORK_POLLER_FUNC(Init)(&poller);
    FLAT_VECTOR_FUNC(Network_Event, Init)(&events);
    NETWORK_ADDRESS_FUNC(IPv4_Loopback)(&address);
    NETWORK_ADDRESS_FUNC(Endpoint_Create)(&endpoint, &address, 0);

    status = NETWORK_LISTENER_FUNC(ListenAt)(&listener, &endpoint, NULL);
    if (status == STATUS_CONST(SUCCESS))
        status = NETWORK_LISTENER_FUNC(LocalEndpoint)(&listener, &endpoint);
    if (status == STATUS_CONST(SUCCESS))
        status = NETWORK_POLLER_FUNC(AddListener)(
            &poller, &listener, NETWORK_EVENT_CONST(ACCEPT),
            NULL, &registration, NULL);
    if (status == STATUS_CONST(SUCCESS))
        status = NETWORK_CONNECTION_FUNC(Connect)(
            &client, "127.0.0.1", endpoint.port, NULL);
    if (status == STATUS_CONST(SUCCESS))
        status = NETWORK_POLLER_FUNC(WaitFor)(
            &poller, &events, timeout, NULL);

    int ready = status == STATUS_CONST(SUCCESS) &&
                events.size > 0 &&
                NETWORK_POLLER_FUNC(Event_Has)(
                    &events.data[0], NETWORK_EVENT_CONST(ACCEPT));

    FLAT_VECTOR_FUNC(Network_Event, Destroy)(&events);
    NETWORK_POLLER_FUNC(Destroy)(&poller);
    NETWORK_CONNECTION_FUNC(Destroy)(&client);
    NETWORK_LISTENER_FUNC(Close)(&listener);
    return ready ? 0 : 1;
}
```

---

# Lifecycle and ownership

| Object | Initialization | Final cleanup | Owned resources |
| --- | --- | --- | --- |
| `NETWORK_ADDRESS_TYPE(TAddress)` | IPv4/IPv6 value helper | None | None |
| `NETWORK_RESOLVE_TYPE(TAddressVector)` | `FLAT_VECTOR_FUNC(Network_Address, Init)` | `FLAT_VECTOR_FUNC(Network_Address, Destroy)` | Address array |
| `SOCKET_TYPE(TSocket)` | `SOCKET_FUNC(Init)` | `SOCKET_FUNC(Close)` | Native descriptor/socket |
| `NETWORK_CONNECTION_TYPE(TConnection)` | `NETWORK_CONNECTION_FUNC(Init)` | `NETWORK_CONNECTION_FUNC(Destroy)` | Socket and copied hostname |
| `NETWORK_DATAGRAM_TYPE(TDatagram)` | `NETWORK_DATAGRAM_FUNC(Init)` | `NETWORK_DATAGRAM_FUNC(Close)` | UDP socket |
| `NETWORK_LISTENER_TYPE(TListener)` | `NETWORK_LISTENER_FUNC(Init)` | `NETWORK_LISTENER_FUNC(Close)` | One/two listening sockets |
| `NETWORK_POLLER_TYPE(TPoller)` | `NETWORK_POLLER_FUNC(Init)` | `NETWORK_POLLER_FUNC(Destroy)` | Registration vector; **not** registered objects |
| `NETWORK_POLLER_TYPE(TEventVector)` | `FLAT_VECTOR_FUNC(Network_Event, Init)` | `FLAT_VECTOR_FUNC(Network_Event, Destroy)` | Event array; **not** referenced objects |

Avoid shallow copying initialized socket-owning objects and subsequently closing both copies: this would duplicate the native handle identity. Destroy or unregister before releasing objects still observed by a poller.

---

# Blocking, timeouts and event loop behavior

**TCP is a byte stream.** There are no built-in message boundaries. `Read`/`Write` (and Socket Receive/Send) may transfer fewer bytes than requested; `ReadExact`/`WriteAll` loop but are most appropriate for blocking operation. With nonblocking sockets, WOULD_BLOCK may interrupt them **after partial transfer**, without reporting cumulative progress.

`Connection_ConnectFor` has a timeout for establishing a connection. It does **not** establish a general timeout for all subsequent Read/Write calls.

The poller watches configured events using `select` on POSIX or `WSAPoll` on Windows; it is neither epoll nor kqueue. On POSIX, handles outside `FD_SETSIZE` are rejected with `STATUS_CONST(OUT_OF_RANGE)` and a resource-specific error. Timeouts in `WaitFor` return SUCCESS with zero events rather than `NETWORK_ERROR_CONST(TIMEOUT)`. An empty registration set also returns immediately.

A closed stream may be reported as CLOSE rather than READ when close detection is enabled, but precise CLOSE/ERROR visibility varies by platform and requested interest flags. Check the event's flags before attempting an operation.

---

# Portability and build

The public headers use C11. The Network tests compile on POSIX with `_POSIX_C_SOURCE=200809L`; this exposes `getaddrinfo` and related declarations under strict C compilation. For example:

```sh
make -C Codespace/Tests/Network strict
make -C Codespace/Tests/Network windows
```

The Windows implementation dynamically loads `ws2_32.dll` using `LoadLibraryA`/`GetProcAddress` and initializes Winsock 2.2. It is designed to avoid an explicit `-lws2_32` link dependency. POSIX implementations use ordinary OS socket calls. This does not remove the requirement for the OS's network runtime or an available network interface.

The tests include TCP, UDP, IPv6, nonblocking behavior, poller descriptor limits, timeout handling, lifecycle, multi-TU and argument validation. Passing the Makefile to a compiler in your environment is a separate validation step; this documentation does not assert that new test runs have completed.

---

# Notes

- All 73 public functions above are extracted from their actual `*_PROTOTYPE` declarations. `Error.h` defines enum values but contains no public functions.
- Use the namespaced function macros rather than the default direct spelling in reusable modules.
- `Network.space`, `Runtime.inc`, Socket `OS.inc` and the `Impl/*.impl` files are implementation facilities, **not** new user-callable packages.
- Returned events, handles, pointers and endpoints have distinct ownership semantics. A poller registration handle is not the native OS socket handle.
- DNS resolution only returns IP bytes, not host/service canonical names, TTL, port or connection establishment.
- Network error enums abstract common OS outcomes, but exact status/error combinations depend on the operation and platform.
