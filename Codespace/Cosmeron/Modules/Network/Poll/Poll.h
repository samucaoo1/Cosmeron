#pragma once

#include "../Connection/Connection.h"
#include "../Datagram/Datagram.h"
#include "../Listener/Listener.h"
#include "../../Container/Array/Vector.h"
#include "../../Chronometry/Duration/Duration.h"

typedef uint32_t TNetwork_Event_Flags;
enum {
  NETWORK_EVENT_CONST(NONE) = 0,
  NETWORK_EVENT_CONST(READ) = 1u << 0,
  NETWORK_EVENT_CONST(WRITE) = 1u << 1,
  NETWORK_EVENT_CONST(ACCEPT) = 1u << 2,
  NETWORK_EVENT_CONST(CLOSE) = 1u << 3,
  NETWORK_EVENT_CONST(ERROR) = 1u << 4
};

typedef enum NETWORK_POLLER_TYPE(SourceType) {
  NETWORK_POLLER_SOURCE_CONST(CONNECTION),
  NETWORK_POLLER_SOURCE_CONST(LISTENER),
  NETWORK_POLLER_SOURCE_CONST(DATAGRAM)
} TNetwork_Event_Source_Type;

typedef uint64_t TNetwork_Poll_Handle;

typedef struct NETWORK_POLLER_TYPE(TRegistration) {
  TNetwork_Poll_Handle handle;
  TNetwork_Event_Source_Type sourceType;
  TNetwork_Event_Flags interests;
  void *object;
  void *userdata;
} TNetwork_Poll_Registration;

typedef struct NETWORK_POLLER_TYPE(TEvent) {
  TNetwork_Poll_Handle handle;
  TNetwork_Event_Source_Type sourceType;
  TNetwork_Event_Flags flags;
  void *object;
  void *userdata;
} TNetwork_Event;

FLAT_VECTOR_DECLARE(TNetwork_Poll_Registration, Network_Poll_Registration)
FLAT_VECTOR_DECLARE(TNetwork_Event, Network_Event)

typedef FLAT_VECTOR_TYPE(Network_Poll_Registration) TNetwork_Poll_Registration_Vector;
typedef FLAT_VECTOR_TYPE(Network_Event) TNetwork_Event_Vector;

typedef struct NETWORK_POLLER_TYPE(TPoller) {
  TNetwork_Poll_Registration_Vector registrations;
  TNetwork_Poll_Handle nextHandle;
  TNetwork_Error lastError;
} TNetwork_Poller;

inline static void NETWORK_POLLER_FUNC(Init)(TNetwork_Poller *poller);
inline static void NETWORK_POLLER_FUNC(Destroy)(TNetwork_Poller *poller);
inline static OPSTATUS NETWORK_POLLER_FUNC(AddConnection)(
    TNetwork_Poller *poller, TNetwork_Connection *connection,
    TNetwork_Event_Flags interests, void *userdata, TNetwork_Poll_Handle *handle, TNetwork_Error *error);
inline static OPSTATUS NETWORK_POLLER_FUNC(AddListener)(
    TNetwork_Poller *poller, TNetwork_Listener *listener,
    TNetwork_Event_Flags interests, void *userdata, TNetwork_Poll_Handle *handle, TNetwork_Error *error);
inline static OPSTATUS NETWORK_POLLER_FUNC(AddDatagram)(
    TNetwork_Poller *poller, TNetwork_Datagram *datagram,
    TNetwork_Event_Flags interests, void *userdata, TNetwork_Poll_Handle *handle, TNetwork_Error *error);
inline static OPSTATUS NETWORK_POLLER_FUNC(Remove)(
    TNetwork_Poller *poller, TNetwork_Poll_Handle handle,
    TNetwork_Error *error);
inline static OPSTATUS NETWORK_POLLER_FUNC(Watch)(
    TNetwork_Poller *poller, TNetwork_Poll_Handle handle,
    TNetwork_Event_Flags interests, TNetwork_Error *error);
inline static OPSTATUS NETWORK_POLLER_FUNC(Wait)(
    TNetwork_Poller *poller, TNetwork_Event_Vector *events,
    TNetwork_Error *error);
inline static OPSTATUS NETWORK_POLLER_FUNC(WaitFor)(
    TNetwork_Poller *poller, TNetwork_Event_Vector *events,
    CHRONOMETRY_TYPE(Duration) timeout, TNetwork_Error *error);

static inline bool NETWORK_POLLER_FUNC(Event_Has)(
    const TNetwork_Event *event, TNetwork_Event_Flags flags) {
  return (event->flags & flags) == flags;
}
static inline void *NETWORK_POLLER_FUNC(Event_UserData)(
    const TNetwork_Event *event) { return event->userdata; }
static inline TNetwork_Connection *NETWORK_POLLER_FUNC(Event_Connection)(
    const TNetwork_Event *event) {
  return event->sourceType == NETWORK_POLLER_SOURCE_CONST(CONNECTION)
             ? (TNetwork_Connection *)event->object : NULL;
}
static inline TNetwork_Listener *NETWORK_POLLER_FUNC(Event_Listener)(
    const TNetwork_Event *event) {
  return event->sourceType == NETWORK_POLLER_SOURCE_CONST(LISTENER)
             ? (TNetwork_Listener *)event->object : NULL;
}
static inline TNetwork_Datagram *NETWORK_POLLER_FUNC(Event_Datagram)(
    const TNetwork_Event *event) {
  return event->sourceType == NETWORK_POLLER_SOURCE_CONST(DATAGRAM)
             ? (TNetwork_Datagram *)event->object : NULL;
}

#include "Impl/Common.impl"

#if OS_WINDOWS
#include "Impl/Windows.impl"
#else
#include "Impl/POSIX.impl"
#endif
