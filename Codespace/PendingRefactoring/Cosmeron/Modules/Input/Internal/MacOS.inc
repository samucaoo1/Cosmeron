#pragma once
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include "Dynamic.inc"
#include "PhysicalKeys.inc"
#include "MacHID.inc"
#ifndef INPUT_NO_SDL_CONTROLLERS
#include "SDLControllers.inc"
#endif
#include "ControllerTransport.inc"

typedef struct INPUT_INS(MacState) {
  void *graphics, *foundation, *carbon;
  TISInputSourceRef (*copyLayout)(void);
  void *(*layoutProperty)(TISInputSourceRef, CFStringRef);
  const UInt8 *(*dataBytes)(CFDataRef);
  OSStatus (*translate)(const UCKeyboardLayout *, UInt16, UInt16, UInt32, UInt32, OptionBits, UInt32 *, UniCharCount, UniCharCount *, UniChar *);
  UInt8 (*keyboardType)(void);
  CFStringRef layoutPropertyKey;
  INPUT_TYPE(Key) logicalKeys[LIB_PREFIX_CONST(INPUT_KEY_COUNT)];
  CFMachPortRef tap;
  CFRunLoopSourceRef source;
  CFRunLoopRef loop;
  CFStringRef mode;
  bool ready;
  CFMachPortRef (*createTap)(CGEventTapLocation, CGEventTapPlacement, CGEventTapOptions, CGEventMask, CGEventTapCallBack, void *);
  void (*enableTap)(CFMachPortRef, bool);
  bool (*preflight)(void);
  CGEventRef (*createEvent)(CGEventSourceRef);
  CGPoint (*location)(CGEventRef);
  int64_t (*integer)(CGEventRef, CGEventField);
  double (*number)(CGEventRef, CGEventField);
  bool (*key)(CGEventSourceStateID, CGKeyCode);
  bool (*button)(CGEventSourceStateID, CGMouseButton);
  void (*release)(CFTypeRef);
  CFStringRef (*string)(CFAllocatorRef, const char *, CFStringEncoding);
  CFRunLoopRef (*currentLoop)(void);
  CFRunLoopSourceRef (*createSource)(CFAllocatorRef, CFMachPortRef, CFIndex);
  void (*addSource)(CFRunLoopRef, CFRunLoopSourceRef, CFRunLoopMode);
  void (*removeSource)(CFRunLoopRef, CFRunLoopSourceRef, CFRunLoopMode);
  SInt32 (*run)(CFRunLoopMode, CFTimeInterval, Boolean);
} INPUT_INS(MacState);
#if defined(INPUT_EXTERNAL_STORAGE) && !defined(INPUT_DEFINE_STORAGE)
COSMERON_INPUT_STORAGE INPUT_INS(MacState) INPUT_INS(macState);
#else
COSMERON_INPUT_STORAGE INPUT_INS(MacState) INPUT_INS(macState) = {0};
#endif
static inline INPUT_TYPE(Key) INPUT_INS(MacKey)(unsigned native) {
#define COSMERON_INPUT_MAC_KEY(NAME, SCAN, MAC, XNAME) if ((MAC) >= 0 && native == (unsigned)(MAC)) return LIB_PREFIX_CONST(NAME);
  COSMERON_INPUT_PHYSICAL_TABLE(COSMERON_INPUT_MAC_KEY)
#undef COSMERON_INPUT_MAC_KEY
  return LIB_PREFIX_CONST(INPUT_KEY_UNKNOWN);
}
static inline INPUT_TYPE(Key) INPUT_INS(MacLogical)(unsigned code, INPUT_TYPE(Key) physical) {
  INPUT_INS(MacState) *s = &INPUT_INS(macState);
  TISInputSourceRef source;
  CFDataRef data;
  UniChar chars[4]; UniCharCount length = 0; UInt32 dead = 0;
  OSStatus status;
  if (!s->copyLayout || !s->layoutProperty || !s->dataBytes || !s->translate || !s->layoutPropertyKey) return physical;
  if (physical >= LIB_PREFIX_CONST(INPUT_KEY_KEYPAD_0) && physical <= LIB_PREFIX_CONST(INPUT_KEY_KEYPAD_EQUAL)) return physical;
  source = s->copyLayout(); if (!source) return physical;
  data = (CFDataRef)s->layoutProperty(source, s->layoutPropertyKey);
  if (!data) { s->release(source); return physical; }
  status = s->translate((const UCKeyboardLayout *)s->dataBytes(data), (UInt16)code, kUCKeyActionDisplay, 0,
    s->keyboardType ? s->keyboardType() : 0, kUCKeyTranslateNoDeadKeysMask, &dead, 4, &length, chars);
  s->release(source);
  if (status != noErr || length != 1) return physical;
  if (chars[0] >= 'a' && chars[0] <= 'z') return (INPUT_TYPE(Key))(LIB_PREFIX_CONST(INPUT_KEY_A) + chars[0] - 'a');
  if (chars[0] >= 'A' && chars[0] <= 'Z') return (INPUT_TYPE(Key))(LIB_PREFIX_CONST(INPUT_KEY_A) + chars[0] - 'A');
  if (chars[0] >= '0' && chars[0] <= '9') return (INPUT_TYPE(Key))(LIB_PREFIX_CONST(INPUT_KEY_DIGIT_0) + chars[0] - '0');
  switch (chars[0]) {
  case '-': return LIB_PREFIX_CONST(INPUT_KEY_MINUS);
  case '=': return LIB_PREFIX_CONST(INPUT_KEY_EQUAL);
  case '[': return LIB_PREFIX_CONST(INPUT_KEY_LEFT_BRACKET);
  case ']': return LIB_PREFIX_CONST(INPUT_KEY_RIGHT_BRACKET);
  case ';': return LIB_PREFIX_CONST(INPUT_KEY_SEMICOLON);
  case ',': return LIB_PREFIX_CONST(INPUT_KEY_COMMA);
  case '.': return LIB_PREFIX_CONST(INPUT_KEY_PERIOD);
  case '/': return LIB_PREFIX_CONST(INPUT_KEY_SLASH);
  default: return physical; /* unsupported text remains outside this key API */
  }
}
static CGEventRef INPUT_INS(MacEvent)(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *context) {
  INPUT_INS(MacState) *s = &INPUT_INS(macState);
  (void)proxy; (void)context;
  if (type == kCGEventTapDisabledByTimeout || type == kCGEventTapDisabledByUserInput) {
    if (s->tap) s->enableTap(s->tap, true);
    ++INPUT_INS(state).dropped; return event;
  }
  if (type == kCGEventKeyDown || type == kCGEventKeyUp || type == kCGEventFlagsChanged) {
    unsigned code = (unsigned)s->integer(event, kCGKeyboardEventKeycode);
    INPUT_TYPE(Key) physical = INPUT_INS(MacKey)(code);
    INPUT_TYPE(Key) key;
    bool down = type == kCGEventFlagsChanged ? s->key(kCGEventSourceStateCombinedSessionState, (CGKeyCode)code) : type == kCGEventKeyDown;
    bool repeat = type == kCGEventKeyDown && s->integer(event, kCGKeyboardEventAutorepeat) != 0;
    if (down) s->logicalKeys[physical] = INPUT_INS(MacLogical)(code, physical);
    key = s->logicalKeys[physical];
    INPUT_INS(SourceKey)(0, key, physical, down, repeat);
  } else if (type == kCGEventScrollWheel) {
    float x = (float)s->number(event, kCGScrollWheelEventFixedPtDeltaAxis2);
    float y = (float)s->number(event, kCGScrollWheelEventFixedPtDeltaAxis1);
    INPUT_INS(Motion)(x, y, true);
  } else if (type == kCGEventMouseMoved || type == kCGEventLeftMouseDragged || type == kCGEventRightMouseDragged || type == kCGEventOtherMouseDragged) {
    INPUT_INS(Motion)((float)s->integer(event, kCGMouseEventDeltaX), (float)s->integer(event, kCGMouseEventDeltaY), false);
  } else {
    unsigned button = (unsigned)s->integer(event, kCGMouseEventButtonNumber);
    bool down = type == kCGEventLeftMouseDown || type == kCGEventRightMouseDown || type == kCGEventOtherMouseDown;
    if (type == kCGEventLeftMouseDown || type == kCGEventLeftMouseUp || type == kCGEventRightMouseDown || type == kCGEventRightMouseUp || type == kCGEventOtherMouseDown || type == kCGEventOtherMouseUp)
      INPUT_INS(MouseButton)(button, down);
  }
  return event; /* listen-only; never suppress or synthesize OS input */
}
static void INPUT_INS(MacCleanup)(void) {
  INPUT_INS(MacState) *s = &INPUT_INS(macState);
  if (s->source && s->removeSource) s->removeSource(s->loop, s->source, s->mode);
  if (s->tap && s->enableTap) s->enableTap(s->tap, false);
  if (s->release) {
    if (s->source) s->release(s->source);
    if (s->tap) s->release(s->tap);
    if (s->mode) s->release(s->mode);
  }
  if (s->graphics) dlclose(s->graphics);
  if (s->foundation) dlclose(s->foundation);
  if (s->carbon) dlclose(s->carbon);
}
static inline bool INPUT_INS(MacStart)(void) {
  INPUT_INS(MacState) *s = &INPUT_INS(macState);
  if (s->ready) return true;
  s->graphics = dlopen("/System/Library/Frameworks/CoreGraphics.framework/CoreGraphics", RTLD_LOCAL | RTLD_LAZY);
  s->foundation = dlopen("/System/Library/Frameworks/CoreFoundation.framework/CoreFoundation", RTLD_LOCAL | RTLD_LAZY);
  if (!s->graphics || !s->foundation) goto unavailable;
#define COSMERON_INPUT_MAC_LOAD(LIB, FIELD, NAME) COSMERON_INPUT_DLSYM(s->LIB, s->FIELD, NAME); if (!s->FIELD) goto unavailable
  COSMERON_INPUT_MAC_LOAD(graphics, createTap, "CGEventTapCreate");
  COSMERON_INPUT_MAC_LOAD(graphics, enableTap, "CGEventTapEnable");
  COSMERON_INPUT_MAC_LOAD(graphics, createEvent, "CGEventCreate");
  COSMERON_INPUT_MAC_LOAD(graphics, location, "CGEventGetLocation");
  COSMERON_INPUT_MAC_LOAD(graphics, integer, "CGEventGetIntegerValueField");
  COSMERON_INPUT_MAC_LOAD(graphics, number, "CGEventGetDoubleValueField");
  COSMERON_INPUT_MAC_LOAD(graphics, key, "CGEventSourceKeyState");
  COSMERON_INPUT_MAC_LOAD(graphics, button, "CGEventSourceButtonState");
  COSMERON_INPUT_MAC_LOAD(foundation, release, "CFRelease");
  COSMERON_INPUT_MAC_LOAD(foundation, string, "CFStringCreateWithCString");
  COSMERON_INPUT_MAC_LOAD(foundation, currentLoop, "CFRunLoopGetCurrent");
  COSMERON_INPUT_MAC_LOAD(foundation, createSource, "CFMachPortCreateRunLoopSource");
  COSMERON_INPUT_MAC_LOAD(foundation, addSource, "CFRunLoopAddSource");
  COSMERON_INPUT_MAC_LOAD(foundation, removeSource, "CFRunLoopRemoveSource");
  COSMERON_INPUT_MAC_LOAD(foundation, run, "CFRunLoopRunInMode");
#undef COSMERON_INPUT_MAC_LOAD
  COSMERON_INPUT_DLSYM(s->graphics, s->preflight, "CGPreflightListenEventAccess");
  s->carbon = dlopen("/System/Library/Frameworks/Carbon.framework/Frameworks/HIToolbox.framework/HIToolbox", RTLD_LOCAL | RTLD_LAZY);
  if (s->carbon) {
    CFStringRef *property = (CFStringRef *)dlsym(s->carbon, "kTISPropertyUnicodeKeyLayoutData");
    s->layoutPropertyKey = property ? *property : NULL;
    COSMERON_INPUT_DLSYM(s->carbon, s->copyLayout, "TISCopyCurrentKeyboardLayoutInputSource");
    COSMERON_INPUT_DLSYM(s->carbon, s->layoutProperty, "TISGetInputSourceProperty");
    COSMERON_INPUT_DLSYM(s->carbon, s->translate, "UCKeyTranslate");
    COSMERON_INPUT_DLSYM(s->carbon, s->keyboardType, "LMGetKbdType");
    COSMERON_INPUT_DLSYM(s->foundation, s->dataBytes, "CFDataGetBytePtr");
  }
  s->loop = s->currentLoop();
  s->mode = s->string(NULL, "CosmeronInput", kCFStringEncodingUTF8);
  if (!s->mode) goto unavailable;
  if (atexit(INPUT_INS(MacCleanup)) != 0) goto unavailable;
  s->ready = true; return true;
unavailable:
  INPUT_INS(MacCleanup)(); memset(s, 0, sizeof(*s)); return false;
}
static inline OPSTATUS INPUT_INS(BackendUpdate)(void) {
  INPUT_INS(MacState) *s = &INPUT_INS(macState);
  bool available = INPUT_INS(ControllersUpdate)();
  if (INPUT_INS(state).captureMode == INPUT_CONST(CAPTURE_APPLICATION)) return available ? STATUS_CONST(SUCCESS) : STATUS_CONST(NOT_AVAILABLE);
  if (!INPUT_INS(MacStart)()) return available ? STATUS_CONST(SUCCESS) : STATUS_CONST(NOT_AVAILABLE);
  /* Respect Input Monitoring permissions; never request them automatically. */
  if (!s->tap && (!s->preflight || s->preflight())) {
    CGEventMask mask = CGEventMaskBit(kCGEventKeyDown) | CGEventMaskBit(kCGEventKeyUp) | CGEventMaskBit(kCGEventFlagsChanged) |
      CGEventMaskBit(kCGEventLeftMouseDown) | CGEventMaskBit(kCGEventLeftMouseUp) | CGEventMaskBit(kCGEventRightMouseDown) | CGEventMaskBit(kCGEventRightMouseUp) |
      CGEventMaskBit(kCGEventOtherMouseDown) | CGEventMaskBit(kCGEventOtherMouseUp) | CGEventMaskBit(kCGEventMouseMoved) | CGEventMaskBit(kCGEventLeftMouseDragged) |
      CGEventMaskBit(kCGEventRightMouseDragged) | CGEventMaskBit(kCGEventOtherMouseDragged) | CGEventMaskBit(kCGEventScrollWheel);
    s->tap = s->createTap(kCGSessionEventTap, kCGHeadInsertEventTap, kCGEventTapOptionListenOnly, mask, INPUT_INS(MacEvent), NULL);
    if (s->tap) {
      s->source = s->createSource(NULL, s->tap, 0);
      if (s->source) s->addSource(s->loop, s->source, s->mode);
      else { s->release(s->tap); s->tap = NULL; }
    }
  }
  if (s->tap) {
    unsigned budget = 64;
    while (budget-- && s->run(s->mode, 0, true) == kCFRunLoopRunHandledSource) { }
    INPUT_INS(state).capabilities |= INPUT_CONST(CAPABILITY_KEYBOARD) | INPUT_CONST(CAPABILITY_PHYSICAL_KEYS) | INPUT_CONST(CAPABILITY_SCROLL) | INPUT_CONST(CAPABILITY_PRECISE_SCROLL);
    if (s->copyLayout && s->layoutProperty && s->dataBytes && s->translate && s->layoutPropertyKey) INPUT_INS(state).capabilities |= INPUT_CONST(CAPABILITY_LOGICAL_KEYS);
    available = true;
  }
  {
    CGEventRef event = s->createEvent(NULL);
    unsigned b;
    if (event) {
      CGPoint point = s->location(event);
      INPUT_INS(state).x = (float)point.x; INPUT_INS(state).y = (float)point.y;
      s->release(event); INPUT_INS(state).capabilities |= INPUT_CONST(CAPABILITY_POINTER_POSITION);
      available = true;
    }
    for (b = 0; b < INPUT_CONST(MOUSE_BUTTON_COUNT); ++b) INPUT_INS(MouseButton)(b, s->button(kCGEventSourceStateCombinedSessionState, (CGMouseButton)b));
    INPUT_INS(state).capabilities |= INPUT_CONST(CAPABILITY_MOUSE_BUTTONS);
  }
  return available ? STATUS_CONST(SUCCESS) : STATUS_CONST(NOT_AVAILABLE);
}
static inline OPSTATUS INPUT_INS(BackendRumble)(INPUT_INS(Device) *d, float low, float high, uint32_t ms) {
  return INPUT_INS(ControllersRumble)(d, low, high, ms);
}
