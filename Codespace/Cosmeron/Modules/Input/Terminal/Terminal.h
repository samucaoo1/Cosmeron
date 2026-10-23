#pragma once

#include "../../../Core/Error/Status.h"
#include "../../../Core/Module/Object.h"
#include "../Input.space"
#include "../Internal/Core.inc"

#if defined(INPUT_DISABLE_FUNCTION_TABLE) ||                                    \
    defined(INPUT_TERMINAL_DISABLE_FUNCTION_TABLE)
#ifndef OBJECT_DISABLE_FUNCTION_TABLE
#define OBJECT_DISABLE_FUNCTION_TABLE
#endif
#endif

#define INPUT_TERMINAL_NS(NAME) GNS2(INPUT_NS(Terminal), NAME)
#define INPUT_TERMINAL_CNS(NAME) CNS3(LIB_PREFIX_CONST(INPUT_CMOD), TERMINAL, NAME)
#define INPUT_TERMINAL_INS(NAME) GNS3(INPUT_NS(Terminal), Internal, NAME)

#define INPUT_TERMINAL_TYPE(NAME) INPUT_TERMINAL_NS(NAME)
#define INPUT_TERMINAL_FUNC(NAME) GNS2(INPUT_TERMINAL_TYPE(TTerminalInput), NAME)
#define INPUT_TERMINAL_CONST(NAME) INPUT_TERMINAL_CNS(NAME)

enum {
  INPUT_TERMINAL_CONST(ESCAPE_BUFFER_CAPACITY) = 64U,
  INPUT_TERMINAL_CONST(QUEUE_CAPACITY) = 64U
};

/* Terminal Input Parser and Decoder Object */
OBJECT(INPUT_TERMINAL_TYPE(TTerminalInput),
       FIELDS(char escapeBuffer[64];
              size_t escapeLength;
              bool inEscape;
              bool inCsi;
              bool inSgrMouse;
              INPUT_TYPE(TEvent) eventQueue[64];
              size_t queueHead;
              size_t queueCount;
              bool autoSubmit;
              uint32_t currentModifiers;),
       METHOD(OPSTATUS, Create, (bool autoSubmit)),
       METHOD(OPSTATUS, Destroy, ()),
       METHOD(OPSTATUS, Reset, ()),
       METHOD(OPSTATUS, Feed,
              (const void *bytes, size_t size, size_t *outConsumed)),
       METHOD(OPSTATUS, FeedByte, (uint8_t byte)),
       METHOD(OPSTATUS, PollEvent,
              (INPUT_TYPE(TEvent) * outEvent, bool *outFound)),
       CONST_METHOD(size_t, PendingEvents, ()),
       CONST_METHOD(bool, InEscapeSequence, ()));

typedef INPUT_TERMINAL_TYPE(TTerminalInput) TTerminalInput;

#include "Impl/Terminal.impl"

/* EOF */
