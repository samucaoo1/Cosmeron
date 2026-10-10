#define INPUT_NO_SDL_CONTROLLERS
#include "../../Cosmeron/Modules/Input/Input.h"
#include <assert.h>
#if defined(__APPLE__) && defined(__MACH__)
static bool hidPresent = true;
static int hidReleaseCount;
static SInt32 mockRun(CFStringRef mode, CFTimeInterval seconds, Boolean once) { (void)mode; (void)seconds; (void)once; return kCFRunLoopRunFinished; }
static CFSetRef mockDevices(IOHIDManagerRef manager) { (void)manager; return (CFSetRef)(uintptr_t)1; }
static void mockApply(CFSetRef set, CFSetApplierFunction callback, void *context) { (void)set; if (hidPresent) callback((const void *)(uintptr_t)1, context); }
static Boolean mockConforms(IOHIDDeviceRef device, uint32_t page, uint32_t usage) { (void)device; return page == 1 && usage == 5; }
static IOReturn mockOpen(IOHIDDeviceRef device, IOOptionBits options) { (void)device; (void)options; return kIOReturnSuccess; }
static CFTypeRef mockRetain(CFTypeRef value) { return value; }
static void mockRelease(CFTypeRef value) { (void)value; ++hidReleaseCount; }
static CFArrayRef mockElements(IOHIDDeviceRef device, CFDictionaryRef match, IOOptionBits options) { (void)device; (void)match; (void)options; return (CFArrayRef)(uintptr_t)2; }
static CFStringRef mockString(CFAllocatorRef allocator, const char *name, CFStringEncoding encoding) { (void)allocator; (void)name; (void)encoding; return NULL; }
static CFIndex mockCount(CFArrayRef array) { (void)array; return 3; }
static const void *mockElement(CFArrayRef array, CFIndex index) { (void)array; return (const void *)(uintptr_t)(index + 1); }
static IOHIDElementType mockType(IOHIDElementRef element) { return (uintptr_t)element == 1 ? kIOHIDElementTypeInput_Button : kIOHIDElementTypeInput_Misc; }
static uint32_t mockPage(IOHIDElementRef element) { return (uintptr_t)element == 1 ? 9 : 1; }
static uint32_t mockUsage(IOHIDElementRef element) { return (uintptr_t)element == 1 ? 1 : ((uintptr_t)element == 2 ? 0x30 : 0x39); }
static Boolean mockRelative(IOHIDElementRef element) { (void)element; return false; }
static CFIndex mockMinimum(IOHIDElementRef element) { return (uintptr_t)element == 2 ? -32768 : 0; }
static CFIndex mockMaximum(IOHIDElementRef element) { return (uintptr_t)element == 2 ? 32767 : ((uintptr_t)element == 3 ? 7 : 1); }
static IOReturn mockValue(IOHIDDeviceRef device, IOHIDElementRef element, IOHIDValueRef *value) { (void)device; *value = (IOHIDValueRef)element; return kIOReturnSuccess; }
static CFIndex mockInteger(IOHIDValueRef value) { return (uintptr_t)value == 2 ? 32767 : 1; }
#endif
int main(void) {
#if defined(__APPLE__) && defined(__MACH__)
  Input_Internal_MacHIDState *s = &Input_Internal_macHID;
  Input_ControllerId id;
  s->ready = true; s->run = mockRun; s->devices = mockDevices; s->setApply = mockApply;
  s->conforms = mockConforms; s->deviceOpen = mockOpen; s->deviceClose = mockOpen;
  s->retain = mockRetain; s->release = mockRelease; s->elements = mockElements;
  s->string = mockString; s->arrayCount = mockCount; s->arrayValue = mockElement;
  s->type = mockType; s->page = mockPage; s->usage = mockUsage; s->relative = mockRelative;
  s->minimum = mockMinimum; s->maximum = mockMaximum; s->value = mockValue; s->integer = mockInteger;
  assert(Input_Internal_NativeControllersUpdate());
  id = Input_Internal_state.devices[0].id;
  assert(id && Input_Controller_GetButtonCount(id) == 1 && Input_Controller_GetAxisCount(id) == 1);
  assert(Input_Controller_IsDown(id, 0));
  assert(Input_Controller_GetAxis(id, 0) == 1);
  assert(Input_Controller_GetHat(id, 0) == (INPUT_HAT_UP | INPUT_HAT_RIGHT));
  assert(!Input_Gamepad_IsMapped(id) && !Input_Controller_HasRumble(id));
  assert(Input_Internal_NativeControllersUpdate() && Input_Internal_state.devices[0].id == id);
  hidPresent = false;
  assert(!Input_Internal_NativeControllersUpdate());
  assert(!Input_Internal_state.devices[0].connected && hidReleaseCount >= 4);
#endif
  return 0;
}
