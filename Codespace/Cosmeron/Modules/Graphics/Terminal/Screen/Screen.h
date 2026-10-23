#pragma once

#include "../Terminal.space"
#include "../Terminal.h"
#include "../../../../Core/Error/Status.h"
#include "../../../../Core/Module/Object.h"
#include "../../../Text/Grid/Grid.h"
#include "../../../Text/Encoding/UTF8.h"

#if defined(TERMINAL_DISABLE_FUNCTION_TABLE) ||                                \
    defined(TERMINAL_SCREEN_DISABLE_FUNCTION_TABLE)
#ifndef OBJECT_DISABLE_FUNCTION_TABLE
#define OBJECT_DISABLE_FUNCTION_TABLE
#endif
#endif

#define TERMINAL_SCREEN_NS(NAME) GNS2(TERMINAL_NS(Screen), NAME)
#define TERMINAL_SCREEN_CNS(NAME) CNS3(LIB_PREFIX_CONST(TERMINAL_CMOD), SCREEN, NAME)
#define TERMINAL_SCREEN_INS(NAME) GNS3(TERMINAL_NS(Screen), Internal, NAME)

#define TERMINAL_SCREEN_TYPE(NAME) TERMINAL_SCREEN_NS(NAME)
#define TERMINAL_SCREEN_FUNC(NAME) GNS2(TERMINAL_SCREEN_TYPE(TTerminalScreen), NAME)
#define TERMINAL_SCREEN_CONST(NAME) TERMINAL_SCREEN_CNS(NAME)

/* Object-oriented Terminal Screen Buffer definition (Double-buffered) */
OBJECT(TERMINAL_SCREEN_TYPE(TTerminalScreen),
       FIELDS(TEXT_GRID_TYPE(char32) front;
              TEXT_GRID_TYPE(char32) back;
              TDUAL_TYPE(uint16) size;
              bool isAlternate;
              bool isRaw;
              bool isInitialized;),
       METHOD(OPSTATUS, Create,
              (TDUAL_TYPE(uint16) size, bool useAlternateScreen,
               bool useRawMode)),
       METHOD(OPSTATUS, Destroy, ()),
       METHOD(OPSTATUS, Resize, (TDUAL_TYPE(uint16) newSize)),
       METHOD(OPSTATUS, Clear, ()),
       METHOD(OPSTATUS, ClearWith,
              (TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute, uint32_t codepoint)),
       METHOD(OPSTATUS, SetCell,
              (uint16_t col, uint16_t row, uint32_t codepoint,
               TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute)),
       METHOD(OPSTATUS, GetCell,
              (uint16_t col, uint16_t row, uint32_t * outCodepoint,
               TEXT_GRID_ATTRIBUTE_TYPE(Cell) * outAttribute)),
       METHOD(OPSTATUS, WriteString,
              (uint16_t col, uint16_t row, const char *text,
               TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute)),
       METHOD(OPSTATUS, Present, ()),
       CONST_METHOD(TDUAL_TYPE(uint16), Size, ()),
       CONST_METHOD(bool, IsAlternate, ()),
       CONST_METHOD(bool, IsRaw, ()),
       CONST_METHOD(bool, IsInitialized, ()));

typedef TERMINAL_SCREEN_TYPE(TTerminalScreen) TTerminalScreen;

#include "Impl/Screen.impl"

/* EOF */
