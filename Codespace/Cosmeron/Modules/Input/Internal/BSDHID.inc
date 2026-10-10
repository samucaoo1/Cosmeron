#pragma once
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#if defined(__DragonFly__)
#include <bus/u4b/usb.h>
#include <bus/u4b/usb_ioctl.h>
#else
#include <dev/usb/usb.h>
#endif
#include <usbhid.h>
#include "Dynamic.inc"
#include "HID.inc"
enum { INPUT_INS(BSDHIDFields) = INPUT_CONST(MAX_BUTTONS) + INPUT_CONST(MAX_AXES) + INPUT_CONST(MAX_HATS), INPUT_INS(BSDHIDReportSize) = 4096 };
typedef struct INPUT_INS(BSDHIDDevice) {
  bool used;
  int fd, reportSize;
  dev_t device;
  ino_t inode;
  hid_item_t fields[INPUT_INS(BSDHIDFields)];
  unsigned kinds[INPUT_INS(BSDHIDFields)], indices[INPUT_INS(BSDHIDFields)], count;
} INPUT_INS(BSDHIDDevice);
typedef struct INPUT_INS(BSDHIDState) {
  void *library;
  bool ready, scanned;
  time_t scanTime;
  report_desc_t (*descriptor)(int);
  void (*dispose)(report_desc_t);
  hid_data_t (*parse)(report_desc_t, int, int);
  void (*end)(hid_data_t);
  int (*item)(hid_data_t, hid_item_t *);
  int (*size)(report_desc_t, enum hid_kind, int);
  INPUT_INS(BSDHIDDevice) slots[INPUT_DEVICE_CAPACITY];
} INPUT_INS(BSDHIDState);
#if defined(INPUT_EXTERNAL_STORAGE) && !defined(INPUT_DEFINE_STORAGE)
COSMERON_INPUT_STORAGE INPUT_INS(BSDHIDState) INPUT_INS(bsdHID);
#else
COSMERON_INPUT_STORAGE INPUT_INS(BSDHIDState) INPUT_INS(bsdHID) = {0};
#endif
static inline void INPUT_INS(BSDHIDClose)(size_t slot) {
  INPUT_INS(BSDHIDDevice) *n = &INPUT_INS(bsdHID).slots[slot];
  if (!n->used) return;
  close(n->fd); memset(n, 0, sizeof(*n)); INPUT_INS(Disconnect)(slot);
}
static void INPUT_INS(BSDHIDCleanup)(void) {
  size_t i;
  for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i) INPUT_INS(BSDHIDClose)(i);
  if (INPUT_INS(bsdHID).library) dlclose(INPUT_INS(bsdHID).library);
}
static inline bool INPUT_INS(BSDHIDStart)(void) {
  INPUT_INS(BSDHIDState) *s = &INPUT_INS(bsdHID);
  if (s->ready) return true;
  s->library = dlopen("libusbhid.so", RTLD_LOCAL | RTLD_LAZY);
  if (!s->library) return false;
#define COSMERON_INPUT_BSD_LOAD(FIELD, NAME) COSMERON_INPUT_DLSYM(s->library, s->FIELD, NAME); if (!s->FIELD) goto unavailable
  COSMERON_INPUT_BSD_LOAD(descriptor, "hid_get_report_desc");
  COSMERON_INPUT_BSD_LOAD(dispose, "hid_dispose_report_desc");
  COSMERON_INPUT_BSD_LOAD(parse, "hid_start_parse");
  COSMERON_INPUT_BSD_LOAD(end, "hid_end_parse");
  COSMERON_INPUT_BSD_LOAD(item, "hid_get_item");
  COSMERON_INPUT_BSD_LOAD(size, "hid_report_size");
#undef COSMERON_INPUT_BSD_LOAD
  if (atexit(INPUT_INS(BSDHIDCleanup)) != 0) goto unavailable;
  s->ready = true; return true;
unavailable:
  dlclose(s->library); memset(s, 0, sizeof(*s)); return false;
}
static inline void INPUT_INS(BSDHIDOpen)(const char *path) {
  INPUT_INS(BSDHIDState) *s = &INPUT_INS(bsdHID);
  INPUT_INS(BSDHIDDevice) *n;
  INPUT_INS(Device) *d;
  struct stat st;
  report_desc_t descriptor;
  hid_data_t parser;
  hid_item_t item;
  size_t slot, i;
  int fd, id = 0, length;
  bool controller = false;
  unsigned buttons = 0, axes = 0, hats = 0;
  if (stat(path, &st) < 0 || !S_ISCHR(st.st_mode)) return;
  for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i)
    if (s->slots[i].used && s->slots[i].device == st.st_rdev && s->slots[i].inode == st.st_ino) return;
  for (slot = 0; slot < INPUT_DEVICE_CAPACITY; ++slot) if (!INPUT_INS(state).devices[slot].connected && !s->slots[slot].used) break;
  if (slot == INPUT_DEVICE_CAPACITY) return;
  fd = open(path, O_RDONLY | O_NONBLOCK);
  if (fd < 0) return;
  if (fcntl(fd, F_SETFD, FD_CLOEXEC) < 0 || ioctl(fd, USB_GET_REPORT_ID, &id) < 0) { close(fd); return; }
  descriptor = s->descriptor(fd);
  if (!descriptor) { close(fd); return; }
  length = s->size(descriptor, hid_input, id);
  if (length <= 0 || length > INPUT_INS(BSDHIDReportSize)) { s->dispose(descriptor); close(fd); return; }
  parser = s->parse(descriptor, (1 << hid_input) | (1 << hid_collection) | (1 << hid_endcollection), -1);
  if (!parser) { s->dispose(descriptor); close(fd); return; }
  n = &s->slots[slot]; memset(n, 0, sizeof(*n));
  while (s->item(parser, &item)) {
    uint32_t page = HID_PAGE(item.usage), usage = HID_USAGE(item.usage);
    unsigned kind, index;
    if (item.kind == hid_collection && INPUT_INS(HIDController)(page, usage)) controller = true;
    /* Variable absolute inputs only. Arrays, constants and relative inputs
     * require different semantics and must not become spurious buttons. */
    if (item.kind != hid_input || item.report_ID != id || (item.flags & 7) != 2 || item.report_size <= 0 || item.report_size > 32) continue;
    if ((uint64_t)item.pos + (unsigned)item.report_size > (uint64_t)length * 8) continue;
    if (page == 9 && buttons < INPUT_CONST(MAX_BUTTONS)) { kind = 0; index = buttons++; }
    else if (INPUT_INS(HIDAxis)(page, usage) && axes < INPUT_CONST(MAX_AXES)) { kind = 1; index = axes++; }
    else if (page == 1 && usage == 0x39 && hats < INPUT_CONST(MAX_HATS)) { kind = 2; index = hats++; }
    else continue;
    n->fields[n->count] = item; n->kinds[n->count] = kind; n->indices[n->count++] = index;
    if (n->count == INPUT_INS(BSDHIDFields)) break;
  }
  s->end(parser); s->dispose(descriptor);
  if (!controller || !n->count) { close(fd); memset(n, 0, sizeof(*n)); return; }
  d = INPUT_INS(Connect)(slot, path);
  if (!d) { close(fd); memset(n, 0, sizeof(*n)); return; }
  n->fd = fd; n->used = true; n->reportSize = length; n->device = st.st_rdev; n->inode = st.st_ino;
  d->buttonCount = buttons; d->axisCount = axes; d->hatCount = hats;
}
static inline void INPUT_INS(BSDHIDScan)(void) {
#if defined(__OpenBSD__)
  const char *directory = "/dev/ujoy";
#else
  const char *directory = "/dev";
#endif
  DIR *dir = opendir(directory);
  struct dirent *entry;
  if (!dir) return;
  while ((entry = readdir(dir)) != NULL) {
    char path[512];
    const char *name = entry->d_name;
    const char *number;
#if defined(__OpenBSD__)
    number = name;
#else
    if (strncmp(name, "uhid", 4) != 0) continue;
    number = name + 4;
#endif
    if (!*number) continue;
    while (*number >= '0' && *number <= '9') ++number;
    if (*number) continue;
    if (snprintf(path, sizeof(path), "%s/%s", directory, name) >= (int)sizeof(path)) continue;
    INPUT_INS(BSDHIDOpen)(path);
  }
  closedir(dir);
}
static inline void INPUT_INS(BSDHIDReport)(size_t slot, const unsigned char *report, size_t length) {
  INPUT_INS(BSDHIDDevice) *n = &INPUT_INS(bsdHID).slots[slot];
  INPUT_INS(Device) *d = &INPUT_INS(state).devices[slot];
  unsigned i;
  for (i = 0; i < n->count; ++i) {
    const hid_item_t *field = &n->fields[i];
    int64_t value;
    if (!INPUT_INS(HIDBits)(report, length, field->pos, (unsigned)field->report_size, field->logical_minimum < 0, &value)) continue;
    if (n->kinds[i] == 0) INPUT_INS(ControllerButton)(d, n->indices[i], value != 0);
    else if (n->kinds[i] == 1) INPUT_INS(Axis)(d, n->indices[i], INPUT_INS(HIDNormalize)((double)value, field->logical_minimum, field->logical_maximum));
    else INPUT_INS(Hat)(d, n->indices[i], INPUT_INS(HIDHat)(value, field->logical_minimum, field->logical_maximum));
  }
}
static inline bool INPUT_INS(NativeControllersUpdate)(void) {
  INPUT_INS(BSDHIDState) *s = &INPUT_INS(bsdHID);
  size_t slot;
  time_t now;
  bool available = false;
  if (!INPUT_INS(BSDHIDStart)()) return false;
  now = time(NULL);
  if (!s->scanned || now != s->scanTime) { INPUT_INS(BSDHIDScan)(); s->scanned = true; s->scanTime = now; }
  for (slot = 0; slot < INPUT_DEVICE_CAPACITY; ++slot) {
    INPUT_INS(BSDHIDDevice) *n = &s->slots[slot];
    unsigned budget = 64;
    if (!n->used) continue;
    while (budget--) {
      unsigned char report[INPUT_INS(BSDHIDReportSize)];
      ssize_t length = read(n->fd, report, (size_t)n->reportSize);
      if (length < 0 && errno == EINTR) continue;
      if (length < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) break;
      if (length <= 0) { INPUT_INS(BSDHIDClose)(slot); break; }
      if (length != n->reportSize) { ++INPUT_INS(state).dropped; continue; }
      INPUT_INS(BSDHIDReport)(slot, report, (size_t)length);
    }
    available |= n->used;
  }
  if (available) INPUT_INS(state).capabilities |= INPUT_CONST(CAPABILITY_CONTROLLERS);
  return available;
}
