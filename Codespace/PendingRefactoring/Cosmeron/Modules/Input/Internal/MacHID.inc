#pragma once
#include <IOKit/hid/IOHIDManager.h>
#include "Dynamic.inc"
#include "HID.inc"
/* System frameworks are resolved at runtime to preserve the zero-link API. */
enum { INPUT_INS(HIDElementCapacity) = INPUT_CONST(MAX_BUTTONS) + INPUT_CONST(MAX_AXES) + INPUT_CONST(MAX_HATS) };
typedef struct INPUT_INS(MacHIDDevice) {
  IOHIDDeviceRef device;
  CFArrayRef elements;
  IOHIDElementRef inputs[INPUT_INS(HIDElementCapacity)];
  unsigned kinds[INPUT_INS(HIDElementCapacity)], indices[INPUT_INS(HIDElementCapacity)], count;
  bool seen;
} INPUT_INS(MacHIDDevice);
typedef struct INPUT_INS(MacHIDState) {
  void *kit, *foundation;
  IOHIDManagerRef manager;
  CFRunLoopRef loop;
  CFStringRef mode;
  bool ready;
  IOHIDManagerRef (*create)(CFAllocatorRef, IOOptionBits);
  void (*match)(IOHIDManagerRef, CFArrayRef);
  IOReturn (*open)(IOHIDManagerRef, IOOptionBits);
  IOReturn (*close)(IOHIDManagerRef, IOOptionBits);
  void (*schedule)(IOHIDManagerRef, CFRunLoopRef, CFStringRef);
  void (*unschedule)(IOHIDManagerRef, CFRunLoopRef, CFStringRef);
  CFSetRef (*devices)(IOHIDManagerRef);
  Boolean (*conforms)(IOHIDDeviceRef, uint32_t, uint32_t);
  IOReturn (*deviceOpen)(IOHIDDeviceRef, IOOptionBits);
  IOReturn (*deviceClose)(IOHIDDeviceRef, IOOptionBits);
  CFArrayRef (*elements)(IOHIDDeviceRef, CFDictionaryRef, IOOptionBits);
  IOHIDElementType (*type)(IOHIDElementRef);
  uint32_t (*page)(IOHIDElementRef), (*usage)(IOHIDElementRef);
  CFIndex (*minimum)(IOHIDElementRef), (*maximum)(IOHIDElementRef);
  Boolean (*relative)(IOHIDElementRef);
  IOReturn (*value)(IOHIDDeviceRef, IOHIDElementRef, IOHIDValueRef *);
  CFIndex (*integer)(IOHIDValueRef);
  CFTypeRef (*property)(IOHIDDeviceRef, CFStringRef);
  CFTypeRef (*retain)(CFTypeRef);
  void (*release)(CFTypeRef);
  CFTypeID (*typeID)(CFTypeRef), (*numberID)(void), (*stringID)(void);
  Boolean (*number)(CFNumberRef, CFNumberType, void *);
  Boolean (*text)(CFStringRef, char *, CFIndex, CFStringEncoding);
  CFStringRef (*string)(CFAllocatorRef, const char *, CFStringEncoding);
  CFNumberRef (*newNumber)(CFAllocatorRef, CFNumberType, const void *);
  CFDictionaryRef (*dictionary)(CFAllocatorRef, const void **, const void **, CFIndex, const CFDictionaryKeyCallBacks *, const CFDictionaryValueCallBacks *);
  CFArrayRef (*array)(CFAllocatorRef, const void **, CFIndex, const CFArrayCallBacks *);
  CFIndex (*arrayCount)(CFArrayRef);
  const void *(*arrayValue)(CFArrayRef, CFIndex);
  void (*setApply)(CFSetRef, CFSetApplierFunction, void *);
  CFRunLoopRef (*currentLoop)(void);
  SInt32 (*run)(CFStringRef, CFTimeInterval, Boolean);
  INPUT_INS(MacHIDDevice) slots[INPUT_DEVICE_CAPACITY];
} INPUT_INS(MacHIDState);
#if defined(INPUT_EXTERNAL_STORAGE) && !defined(INPUT_DEFINE_STORAGE)
COSMERON_INPUT_STORAGE INPUT_INS(MacHIDState) INPUT_INS(macHID);
#else
COSMERON_INPUT_STORAGE INPUT_INS(MacHIDState) INPUT_INS(macHID) = {0};
#endif
static inline void INPUT_INS(MacHIDClose)(size_t slot) {
  INPUT_INS(MacHIDState) *s = &INPUT_INS(macHID);
  INPUT_INS(MacHIDDevice) *n = &s->slots[slot];
  if (!n->device) return;
  if (n->elements) s->release(n->elements);
  (void)s->deviceClose(n->device, 0); s->release(n->device);
  memset(n, 0, sizeof(*n)); INPUT_INS(Disconnect)(slot);
}
static void INPUT_INS(MacHIDCleanup)(void) {
  INPUT_INS(MacHIDState) *s = &INPUT_INS(macHID);
  size_t i;
  for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i) INPUT_INS(MacHIDClose)(i);
  if (s->manager) {
    if (s->loop && s->mode) s->unschedule(s->manager, s->loop, s->mode);
    (void)s->close(s->manager, 0); s->release(s->manager);
  }
  if (s->mode) s->release(s->mode);
  if (s->kit) dlclose(s->kit);
  if (s->foundation) dlclose(s->foundation);
}
static inline bool INPUT_INS(MacHIDMatch)(void) {
  INPUT_INS(MacHIDState) *s = &INPUT_INS(macHID);
  CFStringRef keys[2] = {NULL, NULL};
  CFNumberRef page = NULL, usages[3] = {NULL, NULL, NULL};
  CFDictionaryRef dictionaries[3] = {NULL, NULL, NULL};
  CFArrayRef array = NULL;
  const int32_t pageValue = 1, usageValues[3] = {4, 5, 8};
  bool success = false;
  unsigned i;
  keys[0] = s->string(NULL, "DeviceUsagePage", kCFStringEncodingUTF8);
  keys[1] = s->string(NULL, "DeviceUsage", kCFStringEncodingUTF8);
  page = s->newNumber(NULL, kCFNumberSInt32Type, &pageValue);
  if (!keys[0] || !keys[1] || !page) goto finish;
  for (i = 0; i < 3; ++i) {
    const void *values[2];
    usages[i] = s->newNumber(NULL, kCFNumberSInt32Type, &usageValues[i]);
    if (!usages[i]) goto finish;
    values[0] = page; values[1] = usages[i];
    /* Objects remain owned until SetDeviceMatchingMultiple has consumed them. */
    dictionaries[i] = s->dictionary(NULL, (const void **)keys, values, 2,
      (const CFDictionaryKeyCallBacks *)dlsym(s->foundation, "kCFTypeDictionaryKeyCallBacks"),
      (const CFDictionaryValueCallBacks *)dlsym(s->foundation, "kCFTypeDictionaryValueCallBacks"));
    if (!dictionaries[i]) goto finish;
  }
  array = s->array(NULL, (const void **)dictionaries, 3,
    (const CFArrayCallBacks *)dlsym(s->foundation, "kCFTypeArrayCallBacks"));
  if (array) { s->match(s->manager, array); success = true; }
finish:
  if (array) s->release(array);
  for (i = 0; i < 3; ++i) { if (dictionaries[i]) s->release(dictionaries[i]); if (usages[i]) s->release(usages[i]); }
  if (page) s->release(page);
  for (i = 0; i < 2; ++i) if (keys[i]) s->release(keys[i]);
  return success;
}
static inline bool INPUT_INS(MacHIDStart)(void) {
  INPUT_INS(MacHIDState) *s = &INPUT_INS(macHID);
  if (s->ready) return true;
  s->kit = dlopen("/System/Library/Frameworks/IOKit.framework/IOKit", RTLD_LOCAL | RTLD_LAZY);
  s->foundation = dlopen("/System/Library/Frameworks/CoreFoundation.framework/CoreFoundation", RTLD_LOCAL | RTLD_LAZY);
  if (!s->kit || !s->foundation) goto unavailable;
#define COSMERON_INPUT_HID_LOAD(LIB, FIELD, NAME) COSMERON_INPUT_DLSYM(s->LIB, s->FIELD, NAME); if (!s->FIELD) goto unavailable
  COSMERON_INPUT_HID_LOAD(kit, create, "IOHIDManagerCreate");
  COSMERON_INPUT_HID_LOAD(kit, match, "IOHIDManagerSetDeviceMatchingMultiple");
  COSMERON_INPUT_HID_LOAD(kit, open, "IOHIDManagerOpen");
  COSMERON_INPUT_HID_LOAD(kit, close, "IOHIDManagerClose");
  COSMERON_INPUT_HID_LOAD(kit, schedule, "IOHIDManagerScheduleWithRunLoop");
  COSMERON_INPUT_HID_LOAD(kit, unschedule, "IOHIDManagerUnscheduleFromRunLoop");
  COSMERON_INPUT_HID_LOAD(kit, devices, "IOHIDManagerCopyDevices");
  COSMERON_INPUT_HID_LOAD(kit, conforms, "IOHIDDeviceConformsTo");
  COSMERON_INPUT_HID_LOAD(kit, deviceOpen, "IOHIDDeviceOpen");
  COSMERON_INPUT_HID_LOAD(kit, deviceClose, "IOHIDDeviceClose");
  COSMERON_INPUT_HID_LOAD(kit, elements, "IOHIDDeviceCopyMatchingElements");
  COSMERON_INPUT_HID_LOAD(kit, type, "IOHIDElementGetType");
  COSMERON_INPUT_HID_LOAD(kit, page, "IOHIDElementGetUsagePage");
  COSMERON_INPUT_HID_LOAD(kit, usage, "IOHIDElementGetUsage");
  COSMERON_INPUT_HID_LOAD(kit, minimum, "IOHIDElementGetLogicalMin");
  COSMERON_INPUT_HID_LOAD(kit, maximum, "IOHIDElementGetLogicalMax");
  COSMERON_INPUT_HID_LOAD(kit, relative, "IOHIDElementIsRelative");
  COSMERON_INPUT_HID_LOAD(kit, value, "IOHIDDeviceGetValue");
  COSMERON_INPUT_HID_LOAD(kit, integer, "IOHIDValueGetIntegerValue");
  COSMERON_INPUT_HID_LOAD(kit, property, "IOHIDDeviceGetProperty");
  COSMERON_INPUT_HID_LOAD(foundation, retain, "CFRetain");
  COSMERON_INPUT_HID_LOAD(foundation, release, "CFRelease");
  COSMERON_INPUT_HID_LOAD(foundation, typeID, "CFGetTypeID");
  COSMERON_INPUT_HID_LOAD(foundation, numberID, "CFNumberGetTypeID");
  COSMERON_INPUT_HID_LOAD(foundation, stringID, "CFStringGetTypeID");
  COSMERON_INPUT_HID_LOAD(foundation, number, "CFNumberGetValue");
  COSMERON_INPUT_HID_LOAD(foundation, text, "CFStringGetCString");
  COSMERON_INPUT_HID_LOAD(foundation, string, "CFStringCreateWithCString");
  COSMERON_INPUT_HID_LOAD(foundation, newNumber, "CFNumberCreate");
  COSMERON_INPUT_HID_LOAD(foundation, dictionary, "CFDictionaryCreate");
  COSMERON_INPUT_HID_LOAD(foundation, array, "CFArrayCreate");
  COSMERON_INPUT_HID_LOAD(foundation, arrayCount, "CFArrayGetCount");
  COSMERON_INPUT_HID_LOAD(foundation, arrayValue, "CFArrayGetValueAtIndex");
  COSMERON_INPUT_HID_LOAD(foundation, setApply, "CFSetApplyFunction");
  COSMERON_INPUT_HID_LOAD(foundation, currentLoop, "CFRunLoopGetCurrent");
  COSMERON_INPUT_HID_LOAD(foundation, run, "CFRunLoopRunInMode");
#undef COSMERON_INPUT_HID_LOAD
  s->manager = s->create(NULL, 0);
  if (!s->manager || !INPUT_INS(MacHIDMatch)()) goto unavailable;
  s->mode = s->string(NULL, "CosmeronControllers", kCFStringEncodingUTF8);
  if (!s->mode) goto unavailable;
  s->loop = s->currentLoop(); s->schedule(s->manager, s->loop, s->mode);
  if (s->open(s->manager, 0) != kIOReturnSuccess) goto unavailable;
  if (atexit(INPUT_INS(MacHIDCleanup)) != 0) goto unavailable;
  s->ready = true; return true;
unavailable:
  INPUT_INS(MacHIDCleanup)(); memset(s, 0, sizeof(*s)); return false;
}
static inline CFTypeRef INPUT_INS(MacHIDProperty)(IOHIDDeviceRef device, const char *name) {
  INPUT_INS(MacHIDState) *s = &INPUT_INS(macHID);
  CFStringRef key = s->string(NULL, name, kCFStringEncodingUTF8);
  CFTypeRef value = key ? s->property(device, key) : NULL;
  if (key) s->release(key);
  return value;
}
static inline uint16_t INPUT_INS(MacHIDIdentifier)(IOHIDDeviceRef device, const char *name) {
  INPUT_INS(MacHIDState) *s = &INPUT_INS(macHID);
  CFTypeRef value = INPUT_INS(MacHIDProperty)(device, name);
  int32_t number = 0;
  if (value && s->typeID(value) == s->numberID()) (void)s->number((CFNumberRef)value, kCFNumberSInt32Type, &number);
  return number > 0 && number <= UINT16_MAX ? (uint16_t)number : 0;
}
static void INPUT_INS(MacHIDDiscover)(const void *object, void *context) {
  INPUT_INS(MacHIDState) *s = &INPUT_INS(macHID);
  IOHIDDeviceRef device = (IOHIDDeviceRef)object;
  INPUT_INS(MacHIDDevice) *n;
  INPUT_INS(Device) *d;
  CFIndex count, i;
  CFTypeRef product;
  size_t slot;
  char name[INPUT_CONST(NAME_CAPACITY)] = "HID controller";
  (void)context;
  for (slot = 0; slot < INPUT_DEVICE_CAPACITY; ++slot) if (s->slots[slot].device == device) { s->slots[slot].seen = true; return; }
  if (!s->conforms(device, 1, 4) && !s->conforms(device, 1, 5) && !s->conforms(device, 1, 8)) return;
  for (slot = 0; slot < INPUT_DEVICE_CAPACITY; ++slot) if (!INPUT_INS(state).devices[slot].connected) break;
  if (slot == INPUT_DEVICE_CAPACITY || s->deviceOpen(device, 0) != kIOReturnSuccess) return;
  n = &s->slots[slot]; n->device = (IOHIDDeviceRef)s->retain(device); n->seen = true;
  n->elements = s->elements(device, NULL, 0);
  if (!n->elements) { INPUT_INS(MacHIDClose)(slot); return; }
  product = INPUT_INS(MacHIDProperty)(device, "Product");
  if (product && s->typeID(product) == s->stringID()) (void)s->text((CFStringRef)product, name, sizeof(name), kCFStringEncodingUTF8);
  d = INPUT_INS(Connect)(slot, name);
  if (!d) { INPUT_INS(MacHIDClose)(slot); return; }
  d->vendor = INPUT_INS(MacHIDIdentifier)(device, "VendorID");
  d->product = INPUT_INS(MacHIDIdentifier)(device, "ProductID");
  count = s->arrayCount(n->elements);
  for (i = 0; i < count && n->count < INPUT_INS(HIDElementCapacity); ++i) {
    IOHIDElementRef element = (IOHIDElementRef)s->arrayValue(n->elements, i);
    IOHIDElementType type = s->type(element);
    uint32_t page = s->page(element), usage = s->usage(element);
    unsigned kind, index;
    if (type != kIOHIDElementTypeInput_Button && type != kIOHIDElementTypeInput_Axis && type != kIOHIDElementTypeInput_Misc) continue;
    if (s->relative(element)) continue;
    if (page == 9 && d->buttonCount < INPUT_CONST(MAX_BUTTONS)) { kind = 0; index = d->buttonCount++; }
    else if (INPUT_INS(HIDAxis)(page, usage) && d->axisCount < INPUT_CONST(MAX_AXES)) { kind = 1; index = d->axisCount++; }
    else if (page == 1 && usage == 0x39 && d->hatCount < INPUT_CONST(MAX_HATS)) { kind = 2; index = d->hatCount++; }
    else continue;
    n->inputs[n->count] = element; n->kinds[n->count] = kind; n->indices[n->count++] = index;
  }
  if (!n->count) INPUT_INS(MacHIDClose)(slot);
}
static inline bool INPUT_INS(NativeControllersUpdate)(void) {
  INPUT_INS(MacHIDState) *s = &INPUT_INS(macHID);
  CFSetRef devices;
  size_t slot;
  unsigned budget = 64;
  bool available = false;
  if (!INPUT_INS(MacHIDStart)()) return false;
  while (budget-- && s->run(s->mode, 0, true) == kCFRunLoopRunHandledSource) { }
  devices = s->devices(s->manager);
  for (slot = 0; slot < INPUT_DEVICE_CAPACITY; ++slot) s->slots[slot].seen = false;
  if (devices) { s->setApply(devices, INPUT_INS(MacHIDDiscover), NULL); s->release(devices); }
  for (slot = 0; slot < INPUT_DEVICE_CAPACITY; ++slot) {
    INPUT_INS(MacHIDDevice) *n = &s->slots[slot];
    INPUT_INS(Device) *d = &INPUT_INS(state).devices[slot];
    unsigned i;
    if (!n->device) continue;
    if (!n->seen) { INPUT_INS(MacHIDClose)(slot); continue; }
    available = true;
    for (i = 0; i < n->count; ++i) {
      IOHIDValueRef value = NULL;
      IOHIDElementRef element = n->inputs[i];
      CFIndex integer, minimum = s->minimum(element), maximum = s->maximum(element);
      IOReturn status = s->value(n->device, element, &value);
      if (status != kIOReturnSuccess || !value) continue;
      integer = s->integer(value);
      if (n->kinds[i] == 0) INPUT_INS(ControllerButton)(d, n->indices[i], integer != 0);
      else if (n->kinds[i] == 1) INPUT_INS(Axis)(d, n->indices[i], INPUT_INS(HIDNormalize)((double)integer, (double)minimum, (double)maximum));
      else INPUT_INS(Hat)(d, n->indices[i], INPUT_INS(HIDHat)(integer, minimum, maximum));
    }
  }
  if (available) INPUT_INS(state).capabilities |= INPUT_CONST(CAPABILITY_CONTROLLERS);
  return available;
}
