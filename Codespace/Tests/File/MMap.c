#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include <assert.h>
#include <string.h>
#include <unistd.h>

#include "../../Cosmeron/Modules/File/File.h"

#define OK(x) assert((x) == STATUS_CONST(SUCCESS))

int main(void) {
  char root[128], path[256];
  size_t required;
  unsigned long pid = (unsigned long)getpid();
  assert(snprintf(root, sizeof(root), "cosmeron-mmap-test-%lu", pid) > 0);

  OK(File_Directory_Create(root));
  OK(File_Path_Join(root, "mapped.dat", path, sizeof(path), &required));

  /* Create file with 4096 bytes of dummy data */
  char buffer[4096];
  memset(buffer, 'A', sizeof(buffer));
  memcpy(buffer, "COSMERON_MMAP_TEST", 18);
  OK(File_Save(path, buffer, sizeof(buffer)));

  /* ------------------------------------------------------------
   * 1. Read-only Mapping
   * ------------------------------------------------------------ */
  {
    TMemoryMappedFile mmap = {0};
    OK(FILE_MMAP_FUNC(Init)(&mmap));
    assert(mmap.api != NULL);
    assert(!mmap.api->IsMapped(&mmap));

    OK(mmap.api->Open(&mmap, path, FILE_MMAP_CONST(READ), 0, sizeof(buffer)));
    assert(mmap.api->IsMapped(&mmap));
    assert(!mmap.api->IsWritable(&mmap));
    assert(mmap.api->Size(&mmap) == sizeof(buffer));
    assert(mmap.api->Data(&mmap) != NULL);

    /* Verify content */
    assert(memcmp(mmap.api->Data(&mmap), "COSMERON_MMAP_TEST", 18) == 0);

    /* Lock and Unlock */
    OK(mmap.api->Lock(&mmap));
    OK(mmap.api->Unlock(&mmap));

    OK(mmap.api->Close(&mmap));
    assert(!mmap.api->IsMapped(&mmap));
  }

  /* ------------------------------------------------------------
   * 2. Read-Write Mapping and Sync
   * ------------------------------------------------------------ */
  {
    TMemoryMappedFile mmap = {0};
    OK(FILE_MMAP_FUNC(Init)(&mmap));

    OK(mmap.api->Open(&mmap, path, FILE_MMAP_CONST(READ_WRITE), 0, sizeof(buffer)));
    assert(mmap.api->IsMapped(&mmap));
    assert(mmap.api->IsWritable(&mmap));

    char *data = (char *)mmap.api->Data(&mmap);
    assert(data != NULL);

    /* Mutate mapped memory */
    memcpy(data, "UPDATED_MMAP_DATA!", 18);
    OK(mmap.api->Sync(&mmap, FILE_MMAP_CONST(SYNC_SYNC)));

    OK(mmap.api->Close(&mmap));
  }

  /* ------------------------------------------------------------
   * 3. Verify Mutation via File_Load
   * ------------------------------------------------------------ */
  {
    void *loadedData = NULL;
    size_t loadedSize = 0;
    OK(File_Load(path, &loadedData, &loadedSize));
    assert(loadedSize == sizeof(buffer));
    assert(memcmp(loadedData, "UPDATED_MMAP_DATA!", 18) == 0);
    free(loadedData);
  }

  /* ------------------------------------------------------------
   * 4. Partial / Offset Mapping
   * ------------------------------------------------------------ */
  {
    TMemoryMappedFile mmap = {0};
    OK(FILE_MMAP_FUNC(Init)(&mmap));

    /* Map 64 bytes at offset 0 */
    OK(mmap.api->Open(&mmap, path, FILE_MMAP_CONST(READ), 0, 64));
    assert(mmap.api->Size(&mmap) == 64);
    assert(memcmp(mmap.api->Data(&mmap), "UPDATED_MMAP_DATA!", 18) == 0);
    OK(mmap.api->Close(&mmap));
  }

  /* Cleanup temporary test files */
  OK(File_Remove(path));
  OK(File_Directory_Remove(root));

  return 0;
}
