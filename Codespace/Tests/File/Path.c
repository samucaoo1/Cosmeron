#include <assert.h>
#include <string.h>
#include "../../Cosmeron/Modules/File/Path/Path.h"
#define OK(x) assert((x) == STATUS_CONST(SUCCESS))
int main(void) {
  char output[256] = "unchanged"; size_t required = 99;
  OK(File_Path_Join("assets", "font.flf", output, sizeof(output), &required));
  assert(!strcmp(output, "assets/font.flf") && required == strlen(output) + 1);
  strcpy(output, "unchanged");
  assert(File_Path_Join("abc", "def", output, 2, &required) == STATUS_CONST(OUT_OF_RANGE));
  assert(!strcmp(output, "unchanged") && required == 8);
  assert(File_Path_Normalize("./a//b/../c/.", NULL, 0, &required) == STATUS_CONST(OUT_OF_RANGE));
  OK(File_Path_Normalize("./a//b/../c/.", output, sizeof(output), &required));
  assert(!strcmp(output, "a/b/../c"));
  strcpy(output, "a//b/.");
  OK(File_Path_Normalize(output, output, sizeof(output), &required));
  assert(!strcmp(output, "a/b"));
  OK(File_Path_Normalize(".", output, sizeof(output), &required)); assert(!strcmp(output, "."));
  OK(File_Path_Parent("/a/b///", output, sizeof(output), &required)); assert(!strcmp(output, "/a"));
  OK(File_Path_Parent("/a", output, sizeof(output), &required)); assert(!strcmp(output, "/"));
  OK(File_Path_Parent("a", output, sizeof(output), &required)); assert(!strcmp(output, "."));
  OK(File_Path_Parent("/", output, sizeof(output), &required)); assert(!strcmp(output, "/"));
  assert(!strcmp(File_Path_Name("a/b.txt"), "b.txt"));
  assert(!strcmp(File_Path_Extension("a/b.tar.gz"), ".gz"));
  assert(!strcmp(File_Path_Extension("a/.hidden"), ""));
  assert(!strcmp(File_Path_Extension(".."), ""));
  assert(!File_Path_Name(NULL) && !File_Path_Extension(NULL));
  assert(!File_Path_IsAbsolute("a") && !File_Path_IsAbsolute(NULL));
#ifdef _WIN32
  assert(File_Path_IsAbsolute("C:\\a") && File_Path_IsAbsolute("\\\\server\\share"));
  assert(!File_Path_IsAbsolute("C:a"));
  OK(File_Path_Normalize("C:\\a\\b", output, sizeof(output), &required)); assert(!strcmp(output, "C:/a/b"));
  OK(File_Path_Parent("C:\\a", output, sizeof(output), &required)); assert(!strcmp(output, "C:\\"));
  OK(File_Path_Parent("\\\\server\\share\\a", output, sizeof(output), &required)); assert(!strcmp(output, "\\\\server\\share"));
#else
  assert(File_Path_IsAbsolute("/a"));
  OK(File_Path_Normalize("a\\b", output, sizeof(output), &required)); assert(!strcmp(output, "a\\b"));
#endif
  OK(File_Path_Resolve(".", output, sizeof(output), &required)); assert(File_Path_IsAbsolute(output));
  assert(File_Path_Resolve("cosmeron-file-missing-xyz", output, sizeof(output), &required) == STATUS_CONST(NOT_FOUND));
  assert(File_Path_Join(NULL, "a", output, sizeof(output), &required) == STATUS_CONST(INVALID_ARGUMENT));

  /* Test View-based operations */
  Text_TView8 pView = Text_View8_FromCStr("/usr/local/bin/script.tar.gz");
  assert(Text_View8_EqualCStr(File_Path_ViewName(pView), "script.tar.gz"));
  assert(Text_View8_EqualCStr(File_Path_ViewExtension(pView), ".gz"));
  assert(Text_View8_EqualCStr(File_Path_ViewStem(pView), "script.tar"));
  assert(Text_View8_EqualCStr(File_Path_ViewParent(pView), "/usr/local/bin"));
  assert(File_Path_ViewIsAbsolute(pView));

  Text_TView8 dotGit = Text_View8_FromCStr(".gitignore");
  assert(Text_View8_EqualCStr(File_Path_ViewName(dotGit), ".gitignore"));
  assert(Text_View8_IsEmpty(File_Path_ViewExtension(dotGit)));
  assert(Text_View8_EqualCStr(File_Path_ViewStem(dotGit), ".gitignore"));
  assert(Text_View8_EqualCStr(File_Path_ViewParent(dotGit), "."));
  assert(!File_Path_ViewIsAbsolute(dotGit));

  return 0;
}
