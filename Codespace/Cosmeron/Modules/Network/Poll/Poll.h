#pragma once

#include "../Connection/Connection.h"
#include "../Datagram/Datagram.h"
#include "../Listener/Listener.h"
#include "../../Container/Array/Vector.h"
#include "../../Chronometry/Duration/Duration.h"

typedef uint32_t NETWORK_POLLER_TYPE(EventFlags);
enum {
  NETWORK_EVENT_CONST(NONE) = 0,
  NETWORK_EVENT_CONST(READ) = 1u << 0,
  NETWORK_EVENT_CONST(WRITE) = 1u << 1,
  NETWORK_EVENT_CONST(ACCEPT) = 1u << 2,
  NETWORK_EVENT_CONST(CLOSE) = 1u << 3,
  NETWORK_EVENT_CONST(ERROR) = 1u << 4
};

typedef enum NETWORK_POLLER_TYPE(TSourceType) {
  NETWORK_POLLER_SOURCE_CONST(CONNECTION),
  NETWORK_POLLER_SOURCE_CONST(LISTENER),
  NETWORK_POLLER_SOURCE_CONST(DATAGRAM)
} NETWORK_POLLER_TYPE(TSourceType);

typedef uint64_t NETWORK_POLLER_TYPE(Handle);

typedef struct NETWORK_POLLER_TYPE(TRegistration) {
  NETWORK_POLLER_TYPE(Handle) handle;
  NETWORK_POLLER_TYPE(TSourceType) sourceType;
  NETWORK_POLLER_TYPE(EventFlags) interests;
  void *object;
  void *userdata;
} NETWORK_POLLER_TYPE(TRegistration);

typedef struct NETWORK_POLLER_TYPE(TEvent) {
  NETWORK_POLLER_TYPE(Handle) handle;
  NETWORK_POLLER_TYPE(TSourceType) sourceType;
  NETWORK_POLLER_TYPE(EventFlags) flags;
  void *object;
  void *userdata;
} NETWORK_POLLER_TYPE(TEvent);

FLAT_VECTOR_IMPLEMENT_ALL(NETWORK_POLLER_TYPE(TRegistration), Network_Poll_Registration)
FLAT_VECTOR_IMPLEMENT_ALL(NETWORK_POLLER_TYPE(TEvent), Network_Event)

typedef FLAT_VECTOR_TYPE(Network_Poll_Registration) NETWORK_POLLER_TYPE(TRegistrationVector);
typedef FLAT_VECTOR_TYPE(Network_Event) NETWORK_POLLER_TYPE(TEventVector);

typedef struct NETWORK_POLLER_TYPE(TPoller) {
  NETWORK_POLLER_TYPE(TRegistrationVector) registrations;
  NETWORK_POLLER_TYPE(Handle) nextHandle;
  NETWORK_TYPE(TError) lastError;
} NETWORK_POLLER_TYPE(TPoller);

static inline void NETWORK_POLLER_FUNC(Init)(NETWORK_POLLER_TYPE(TPoller) *poller);
static inline void NETWORK_POLLER_FUNC(Destroy)(NETWORK_POLLER_TYPE(TPoller) *poller);
static inline OPSTATUS NETWORK_POLLER_FUNC(AddConnection)(
    NETWORK_POLLER_TYPE(TPoller) *poller, NETWORK_CONNECTION_TYPE(TConnection) *connection,
    NETWORK_POLLER_TYPE(EventFlags) interests, void *userdata, NETWORK_POLLER_TYPE(Handle) *handle, NETWORK_TYPE(TError) *error);
static inline OPSTATUS NETWORK_POLLER_FUNC(AddListener)(
    NETWORK_POLLER_TYPE(TPoller) *poller, NETWORK_LISTENER_TYPE(TListener) *listener,
    NETWORK_POLLER_TYPE(EventFlags) interests, void *userdata, NETWORK_POLLER_TYPE(Handle) *handle, NETWORK_TYPE(TError) *error);
static inline OPSTATUS NETWORK_POLLER_FUNC(AddDatagram)(
    NETWORK_POLLER_TYPE(TPoller) *poller, NETWORK_DATAGRAM_TYPE(TDatagram) *datagram,
    NETWORK_POLLER_TYPE(EventFlags) interests, void *userdata, NETWORK_POLLER_TYPE(Handle) *handle, NETWORK_TYPE(TError) *error);
static inline OPSTATUS NETWORK_POLLER_FUNC(Remove)(
    NETWORK_POLLER_TYPE(TPoller) *poller, NETWORK_POLLER_TYPE(Handle) handle,
    NETWORK_TYPE(TError) *error);
static inline OPSTATUS NETWORK_POLLER_FUNC(Watch)(
    NETWORK_POLLER_TYPE(TPoller) *poller, NETWORK_POLLER_TYPE(Handle) handle,
    NETWORK_POLLER_TYPE(EventFlags) interests, NETWORK_TYPE(TError) *error);
static inline OPSTATUS NETWORK_POLLER_FUNC(Wait)(
    NETWORK_POLLER_TYPE(TPoller) *poller, NETWORK_POLLER_TYPE(TEventVector) *events,
    NETWORK_TYPE(TError) *error);
static inline OPSTATUS NETWORK_POLLER_FUNC(WaitFor)(
    NETWORK_POLLER_TYPE(TPoller) *poller, NETWORK_POLLER_TYPE(TEventVector) *events,
    CHRONOMETRY_TYPE(Duration) timeout, NETWORK_TYPE(TError) *error);

static inline bool NETWORK_POLLER_FUNC(Event_Has)(
    const NETWORK_POLLER_TYPE(TEvent) *event, NETWORK_POLLER_TYPE(EventFlags) flags) {
  return (event->flags & flags) == flags;
}
static inline void *NETWORK_POLLER_FUNC(Event_UserData)(
    const NETWORK_POLLER_TYPE(TEvent) *event) { return event->userdata; }
static inline NETWORK_CONNECTION_TYPE(TConnection) *NETWORK_POLLER_FUNC(Event_Connection)(
    const NETWORK_POLLER_TYPE(TEvent) *event) {
  return event->sourceType == NETWORK_POLLER_SOURCE_CONST(CONNECTION)
             ? (NETWORK_CONNECTION_TYPE(TConnection) *)event->object : NULL;
}
static inline NETWORK_LISTENER_TYPE(TListener) *NETWORK_POLLER_FUNC(Event_Listener)(
    const NETWORK_POLLER_TYPE(TEvent) *event) {
  return event->sourceType == NETWORK_POLLER_SOURCE_CONST(LISTENER)
             ? (NETWORK_LISTENER_TYPE(TListener) *)event->object : NULL;
}
static inline NETWORK_DATAGRAM_TYPE(TDatagram) *NETWORK_POLLER_FUNC(Event_Datagram)(
    const NETWORK_POLLER_TYPE(TEvent) *event) {
  return event->sourceType == NETWORK_POLLER_SOURCE_CONST(DATAGRAM)
             ? (NETWORK_DATAGRAM_TYPE(TDatagram) *)event->object : NULL;
}

#include "Impl/Common.impl"

#if OS_WINDOWS
#include "Impl/Windows.impl"
#else
#include "Impl/POSIX.impl"
#endif
