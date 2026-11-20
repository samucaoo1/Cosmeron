#include "../../Cosmeron/Modules/File/File.h"

int main(void) {
  const char message[] = "Hello from Cosmeron!\n";
  char *text = NULL;
  size_t size;
  OPSTATUS status = FILE_FUNC(SaveAtomic)("example.txt", message, sizeof(message) - 1);
  if (status != STATUS_CONST(SUCCESS)) return 1;
  status = FILE_FUNC(LoadText)("example.txt", &text, &size);
  if (status != STATUS_CONST(SUCCESS)) return 1;
  if (fwrite(text, 1, size, stdout) != size) status = STATUS_CONST(GENERIC_ERROR);
  free(text);
  return status == STATUS_CONST(SUCCESS) ? 0 : 1;
}
