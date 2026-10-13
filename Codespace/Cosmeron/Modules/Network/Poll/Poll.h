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
} NETWORK_POLLER_TYPE(TPoller);

#define NETWORK_POLLER_INIT_PROTOTYPE                                      \
  static inline void NETWORK_POLLER_FUNC(Init)(NETWORK_POLLER_TYPE(TPoller) *poller)

#define NETWORK_POLLER_DESTROY_PROTOTYPE                                   \
  static inline void NETWORK_POLLER_FUNC(Destroy)(NETWORK_POLLER_TYPE(TPoller) *poller)

#define NETWORK_POLLER_ADD_CONNECTION_PROTOTYPE                             \
  static inline OPSTATUS NETWORK_POLLER_FUNC(AddConnection)(                 \
      NETWORK_POLLER_TYPE(TPoller) *poller,                                  \
      NETWORK_CONNECTION_TYPE(TConnection) *connection,                     \
      NETWORK_POLLER_TYPE(EventFlags) interests, void *userdata,            \
      NETWORK_POLLER_TYPE(Handle) *handle, NETWORK_TYPE(TError) *error)

#define NETWORK_POLLER_ADD_LISTENER_PROTOTYPE                               \
  static inline OPSTATUS NETWORK_POLLER_FUNC(AddListener)(                   \
      NETWORK_POLLER_TYPE(TPoller) *poller,                                  \
      NETWORK_LISTENER_TYPE(TListener) *listener,                           \
      NETWORK_POLLER_TYPE(EventFlags) interests, void *userdata,            \
      NETWORK_POLLER_TYPE(Handle) *handle, NETWORK_TYPE(TError) *error)

#define NETWORK_POLLER_ADD_DATAGRAM_PROTOTYPE                               \
  static inline OPSTATUS NETWORK_POLLER_FUNC(AddDatagram)(                   \
      NETWORK_POLLER_TYPE(TPoller) *poller,                                  \
      NETWORK_DATAGRAM_TYPE(TDatagram) *datagram,                           \
      NETWORK_POLLER_TYPE(EventFlags) interests, void *userdata,            \
      NETWORK_POLLER_TYPE(Handle) *handle, NETWORK_TYPE(TError) *error)

#define NETWORK_POLLER_REMOVE_PROTOTYPE                                     \
  static inline OPSTATUS NETWORK_POLLER_FUNC(Remove)(                        \
      NETWORK_POLLER_TYPE(TPoller) *poller, NETWORK_POLLER_TYPE(Handle) handle,\
      NETWORK_TYPE(TError) *error)

#define NETWORK_POLLER_WATCH_PROTOTYPE                                      \
  static inline OPSTATUS NETWORK_POLLER_FUNC(Watch)(                         \
      NETWORK_POLLER_TYPE(TPoller) *poller, NETWORK_POLLER_TYPE(Handle) handle,\
      NETWORK_POLLER_TYPE(EventFlags) interests, NETWORK_TYPE(TError) *error)

#define NETWORK_POLLER_WAIT_PROTOTYPE                                       \
  static inline OPSTATUS NETWORK_POLLER_FUNC(Wait)(                          \
      NETWORK_POLLER_TYPE(TPoller) *poller,                                 \
      NETWORK_POLLER_TYPE(TEventVector) *events, NETWORK_TYPE(TError) *error)

#define NETWORK_POLLER_WAIT_FOR_PROTOTYPE                                   \
  static inline OPSTATUS NETWORK_POLLER_FUNC(WaitFor)(                       \
      NETWORK_POLLER_TYPE(TPoller) *poller,                                 \
      NETWORK_POLLER_TYPE(TEventVector) *events,                            \
      CHRONOMETRY_TYPE(Duration) timeout, NETWORK_TYPE(TError) *error)

#define NETWORK_POLLER_EVENT_HAS_PROTOTYPE                                  \
  static inline bool NETWORK_POLLER_FUNC(Event_Has)(                         \
      const NETWORK_POLLER_TYPE(TEvent) *event,                              \
      NETWORK_POLLER_TYPE(EventFlags) flags)

#define NETWORK_POLLER_EVENT_USER_DATA_PROTOTYPE                            \
  static inline OPSTATUS NETWORK_POLLER_FUNC(Event_UserData)(                \
      const NETWORK_POLLER_TYPE(TEvent) *event, void **outUserdata)

#define NETWORK_POLLER_EVENT_CONNECTION_PROTOTYPE                           \
  static inline OPSTATUS NETWORK_POLLER_FUNC(Event_Connection)(              \
      const NETWORK_POLLER_TYPE(TEvent) *event,                              \
      NETWORK_CONNECTION_TYPE(TConnection) **outConnection)

#define NETWORK_POLLER_EVENT_LISTENER_PROTOTYPE                             \
  static inline OPSTATUS NETWORK_POLLER_FUNC(Event_Listener)(                \
      const NETWORK_POLLER_TYPE(TEvent) *event,                              \
      NETWORK_LISTENER_TYPE(TListener) **outListener)

#define NETWORK_POLLER_EVENT_DATAGRAM_PROTOTYPE                             \
  static inline OPSTATUS NETWORK_POLLER_FUNC(Event_Datagram)(                \
      const NETWORK_POLLER_TYPE(TEvent) *event,                              \
      NETWORK_DATAGRAM_TYPE(TDatagram) **outDatagram)

NETWORK_POLLER_INIT_PROTOTYPE;
NETWORK_POLLER_DESTROY_PROTOTYPE;
NETWORK_POLLER_ADD_CONNECTION_PROTOTYPE;
NETWORK_POLLER_ADD_LISTENER_PROTOTYPE;
NETWORK_POLLER_ADD_DATAGRAM_PROTOTYPE;
NETWORK_POLLER_REMOVE_PROTOTYPE;
NETWORK_POLLER_WATCH_PROTOTYPE;
NETWORK_POLLER_WAIT_PROTOTYPE;
NETWORK_POLLER_WAIT_FOR_PROTOTYPE;

NETWORK_POLLER_EVENT_HAS_PROTOTYPE {
  return event != NULL && (event->flags & flags) == flags;
}

NETWORK_POLLER_EVENT_USER_DATA_PROTOTYPE {
  if (event == NULL || outUserdata == NULL)
    return STATUS_CONST(INVALID_ARGUMENT);

  *outUserdata = event->userdata;
  return STATUS_CONST(SUCCESS);
}

NETWORK_POLLER_EVENT_CONNECTION_PROTOTYPE {
  if (event == NULL || outConnection == NULL)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (event->sourceType != NETWORK_POLLER_SOURCE_CONST(CONNECTION))
    return STATUS_CONST(NOT_FOUND);

  *outConnection = (NETWORK_CONNECTION_TYPE(TConnection) *)event->object;
  return STATUS_CONST(SUCCESS);
}

NETWORK_POLLER_EVENT_LISTENER_PROTOTYPE {
  if (event == NULL || outListener == NULL)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (event->sourceType != NETWORK_POLLER_SOURCE_CONST(LISTENER))
    return STATUS_CONST(NOT_FOUND);

  *outListener = (NETWORK_LISTENER_TYPE(TListener) *)event->object;
  return STATUS_CONST(SUCCESS);
}

NETWORK_POLLER_EVENT_DATAGRAM_PROTOTYPE {
  if (event == NULL || outDatagram == NULL)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (event->sourceType != NETWORK_POLLER_SOURCE_CONST(DATAGRAM))
    return STATUS_CONST(NOT_FOUND);

  *outDatagram = (NETWORK_DATAGRAM_TYPE(TDatagram) *)event->object;
  return STATUS_CONST(SUCCESS);
}

#include "Impl/Common.impl"

#if OS_WINDOWS
#include "Impl/Windows.impl"
#else
#include "Impl/POSIX.impl"
#endif
