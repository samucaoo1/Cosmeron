/*
 * ABI-level SQLite threading contract regression. The selected vendor
 * implementation is linked from Backend.c, without separate library flags.
 */
#include "../../Cosmeron/Modules/Database/SQLite/SQLite.h"

int main(void) {
  int actual = cosmeron_database_vendor_sqlite3_threadsafe();
#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || \
    defined(__DragonFly__)
  /* BSD must default to the documented no-pthread zero-link profile. */
  return actual == 0 ? 0 : 1;
#else
  return actual == 1 ? 0 : 1;
#endif
}
