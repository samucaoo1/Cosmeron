#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include <assert.h>
#include "../../Cosmeron/Modules/File/File.h"
#define OK(x) assert((x) == STATUS_CONST(SUCCESS))
int main(void) {
  char root[128], path[256], copy[256], moved[256], nested[256], unicode[256], resolved[1024];
  char bytes[16] = {0}; char *text; void *data;
  size_t count, size, required; uint64_t large;
  bool exists, found; unsigned entries = 0;
  FILE_TYPE(TFile) file = {0}; FILE_TYPE(TDirectory) directory = {0};
  FILE_TYPE(TFileEntry) entry; FILE_TYPE(TFileInfo) info;
#ifdef _WIN32
  unsigned long pid = (unsigned long)GetCurrentProcessId();
#else
  unsigned long pid = (unsigned long)getpid();
#endif
  assert(snprintf(root, sizeof(root), "cosmeron-file-test-%lu", pid) > 0);
  OK(File_Directory_Create(root));
  assert(File_Directory_Create(root) == STATUS_CONST(ALREADY_EXISTS));
  OK(File_Path_Join(root, "data.bin", path, sizeof(path), &required));
  OK(File_Path_Join(root, "copy.bin", copy, sizeof(copy), &required));
  OK(File_Path_Join(root, "moved.bin", moved, sizeof(moved), &required));
  OK(File_Path_Join(root, "a/b", nested, sizeof(nested), &required));
  OK(File_Path_Join(root, "a\xc3\xa7\xc3\xa3o.txt", unicode, sizeof(unicode), &required));
  OK(File_Directory_CreateAll(nested)); OK(File_Directory_CreateAll(nested));
  OK(File_Exists(path, &exists)); assert(!exists);
  OK(File_Save(path, "ab\0c", 4));
  OK(File_Load(path, &data, &size)); assert(size == 4 && !memcmp(data, "ab\0c", 4)); free(data);
  OK(File_Append(path, "de", 2));
  OK(File_LoadText(path, &text, &size)); assert(size == 6 && !memcmp(text, "ab\0cde", 6) && text[size] == 0); free(text);
  OK(File_Open(&file, path, FILE_CONST(READ) | FILE_CONST(WRITE)));
  assert(File_Open(&file, copy, FILE_CONST(READ)) == STATUS_CONST(BUSY));
  OK(File_Read(&file, bytes, 4, &count)); assert(count == 4 && !memcmp(bytes, "ab\0c", 4));
  OK(File_Tell(&file, &large)); assert(large == 4);
  OK(File_Size(&file, &large)); assert(large == 6);
  OK(File_Tell(&file, &large)); assert(large == 4);
  OK(File_Seek(&file, -1, FILE_CONST(SEEK_END)));
  OK(File_Read(&file, bytes, sizeof(bytes), &count)); assert(count == 1 && bytes[0] == 'e');
  OK(File_Read(&file, bytes, sizeof(bytes), &count)); assert(count == 0);
  OK(File_Read(&file, NULL, 0, &count)); assert(count == 0);
  OK(File_Truncate(&file, 3)); OK(File_Size(&file, &large)); assert(large == 3);
  OK(File_Seek(&file, 0, FILE_CONST(SEEK_BEGIN)));
  OK(File_Write(&file, "XYZ", 3, &count)); assert(count == 3);
  OK(File_Write(&file, NULL, 0, &count)); assert(count == 0);
  OK(File_Flush(&file)); OK(File_Sync(&file));
  assert(File_Truncate(&file, UINT64_MAX) == STATUS_CONST(OUT_OF_RANGE));
  OK(File_Close(&file)); OK(File_Close(&file));
  assert(file.native == 0);
  assert(File_Open(&file, path, FILE_CONST(WRITE) | FILE_CONST(CREATE) | FILE_CONST(EXCLUSIVE)) == STATUS_CONST(ALREADY_EXISTS));
  assert(File_Open(&file, path, FILE_CONST(READ) | FILE_CONST(TRUNCATE)) == STATUS_CONST(INVALID_ARGUMENT));
  OK(File_Open(&file, path, FILE_CONST(WRITE) | FILE_CONST(CREATE)));
  OK(File_Size(&file, &large)); assert(large == 3); OK(File_Close(&file));
  OK(File_Info(path, true, &info)); assert(info.kind == FILE_CONST(KIND_REGULAR) && info.size == 3);
  assert(info.available & FILE_CONST(INFO_MODIFIED));
  OK(File_Copy(path, copy, false));
  assert(File_Copy(path, copy, false) == STATUS_CONST(ALREADY_EXISTS));
  assert(File_Copy(path, path, true) == STATUS_CONST(INVALID_ARGUMENT));
  OK(File_LoadText(path, &text, &size)); assert(size == 3 && !strcmp(text, "XYZ")); free(text);
  OK(File_Save(copy, "old", 3)); OK(File_Copy(path, copy, true));
  assert(File_Move(copy, path, false) == STATUS_CONST(ALREADY_EXISTS));
  OK(File_Move(copy, moved, false)); OK(File_Exists(copy, &exists)); assert(!exists);
  {
    unsigned char largeData[40000];
    memset(largeData, 0xa5, sizeof(largeData));
    OK(File_SaveAtomic(copy, largeData, sizeof(largeData)));
    OK(File_Load(copy, &data, &size));
    assert(size == sizeof(largeData) && !memcmp(data, largeData, size)); free(data);
    OK(File_Remove(copy));
    assert(File_SaveAtomic(nested, "cannot replace directory", 24) != STATUS_CONST(SUCCESS));
    OK(File_Info(nested, true, &info)); assert(info.kind == FILE_CONST(KIND_DIRECTORY));
  }
  OK(File_SaveAtomic(path, "new", 3));
  OK(File_LoadText(path, &text, &size)); assert(!strcmp(text, "new")); free(text);
  assert(File_SaveAtomic(path, NULL, 1) == STATUS_CONST(INVALID_ARGUMENT));
  OK(File_LoadText(path, &text, &size)); assert(!strcmp(text, "new")); free(text);
  OK(File_Save(unicode, "UTF8", 4)); OK(File_LoadText(unicode, &text, &size)); assert(!strcmp(text, "UTF8")); free(text);
  OK(File_Path_Resolve(unicode, resolved, sizeof(resolved), &required));
  assert(File_Path_IsAbsolute(resolved));
  assert(File_Directory_CreateAll(path) == STATUS_CONST(ALREADY_EXISTS));
  assert(File_Directory_Remove(root) != STATUS_CONST(SUCCESS));
  OK(File_Directory_Open(&directory, root));
  assert(File_Directory_Open(&directory, root) == STATUS_CONST(BUSY));
  for (;;) {
    OK(File_Directory_Next(&directory, &entry, &found)); if (!found) break;
    assert(strcmp(entry.name, ".") && strcmp(entry.name, ".."));
    assert(entry.kind == FILE_CONST(KIND_REGULAR) || entry.kind == FILE_CONST(KIND_DIRECTORY)); ++entries;
  }
  assert(entries == 4); OK(File_Directory_Next(&directory, &entry, &found)); assert(!found);
  OK(File_Directory_Close(&directory)); OK(File_Directory_Close(&directory));
#ifndef _WIN32
  {
    char alias[256], linkpath[256];
    OK(File_Path_Join(root, "alias", alias, sizeof(alias), &required));
    assert(!link(path, alias));
    assert(File_Copy(path, alias, true) == STATUS_CONST(INVALID_ARGUMENT));
    OK(File_LoadText(path, &text, &size)); assert(!strcmp(text, "new")); free(text);
    OK(File_Remove(alias));
    OK(File_Path_Join(root, "symlink", linkpath, sizeof(linkpath), &required));
    assert(!symlink("data.bin", linkpath));
    OK(File_Info(linkpath, false, &info)); assert(info.kind == FILE_CONST(KIND_LINK));
    OK(File_Info(linkpath, true, &info)); assert(info.kind == FILE_CONST(KIND_REGULAR));
    OK(File_Remove(path)); OK(File_Exists(linkpath, &exists)); assert(exists);
    assert(File_Info(linkpath, true, &info) == STATUS_CONST(NOT_FOUND));
    OK(File_Remove(linkpath));
  }
#else
  OK(File_Remove(path));
#endif
  OK(File_Save(path, NULL, 0)); OK(File_LoadText(path, &text, &size)); assert(size == 0 && text[0] == 0); free(text);
  OK(File_Remove(path)); OK(File_Remove(moved)); OK(File_Remove(unicode));
  OK(File_Directory_Remove(nested));
  OK(File_Path_Join(root, "a", nested, sizeof(nested), &required)); OK(File_Directory_Remove(nested));
  OK(File_Directory_Open(&directory, root)); OK(File_Directory_Next(&directory, &entry, &found)); assert(!found); OK(File_Directory_Close(&directory));
  OK(File_Directory_Remove(root));
  count = 77; assert(File_Read(&file, bytes, sizeof(bytes), &count) == STATUS_CONST(INVALID_ARGUMENT)); assert(count == 77);
  text = (char *)"sentinel"; size = 77;
  assert(File_LoadText(path, &text, &size) == STATUS_CONST(NOT_FOUND)); assert(!strcmp(text, "sentinel") && size == 77);
  assert(File_Exists(NULL, &exists) == STATUS_CONST(INVALID_ARGUMENT));
  return 0;
}
