#include "../../Cosmeron/Modules/Text/Text.h"
#include <assert.h>
#include <string.h>

int main(void) {
  /* ------------------------------------------------------------
   * 1. Test via OBJECT API
   * ------------------------------------------------------------ */
  {
    TStringBuilder sb = {0};
    assert(TEXT_BUILDER_FUNC(Init)(&sb) == STATUS_CONST(SUCCESS));
    assert(sb.api != NULL);
    assert(sb.api->Create(&sb, 0) == STATUS_CONST(SUCCESS));
    assert(sb.api->IsEmpty(&sb) == true);
    assert(sb.api->Length(&sb) == 0);
    assert(strcmp(sb.api->CStr(&sb), "") == 0);

    /* Append Char */
    assert(sb.api->AppendChar(&sb, 'C') == STATUS_CONST(SUCCESS));
    assert(sb.api->Length(&sb) == 1);
    assert(strcmp(sb.api->CStr(&sb), "C") == 0);

    /* Append CStr */
    assert(sb.api->AppendCStr(&sb, "osmeron") == STATUS_CONST(SUCCESS));
    assert(sb.api->Length(&sb) == 8);
    assert(strcmp(sb.api->CStr(&sb), "Cosmeron") == 0);

    /* Append Buffer */
    assert(sb.api->AppendBuffer(&sb, " - C11", 6) == STATUS_CONST(SUCCESS));
    assert(strcmp(sb.api->CStr(&sb), "Cosmeron - C11") == 0);

    /* Append View */
    TEXT_VIEW_TYPE(8) view = Text_View8_FromCStr(" Engine");
    assert(sb.api->AppendView(&sb, view) == STATUS_CONST(SUCCESS));
    assert(strcmp(sb.api->CStr(&sb), "Cosmeron - C11 Engine") == 0);

    /* Append Codepoint (UTF-8) */
    assert(sb.api->AppendChar(&sb, ' ') == STATUS_CONST(SUCCESS));
    assert(sb.api->AppendCodepoint(&sb, 0x1F680) == STATUS_CONST(SUCCESS)); // 🚀 rocket emoji

    /* Append Format */
    assert(TEXT_BUILDER_FUNC(AppendFormat)(&sb, " ver %d.%d", 2, 0) == STATUS_CONST(SUCCESS));

    /* Check resulting View */
    TEXT_VIEW_TYPE(8) resultView = sb.api->View(&sb);
    assert(Text_View8_Length(resultView) == sb.api->Length(&sb));
    assert(Text_View8_StartsWithCStr(resultView, "Cosmeron - C11"));

    /* Build TString8 */
    TEXT_STRING_TYPE(8) finalStr;
    assert(sb.api->BuildString(&sb, &finalStr) == STATUS_CONST(SUCCESS));
    const char *finalCStr = NULL;
    assert(Text_String8_CStr(&finalStr, &finalCStr) == STATUS_CONST(SUCCESS));
    assert(strcmp(finalCStr, sb.api->CStr(&sb)) == 0);
    Text_String8_Destroy(&finalStr);

    /* Clear */
    assert(sb.api->Clear(&sb) == STATUS_CONST(SUCCESS));
    assert(sb.api->IsEmpty(&sb) == true);
    assert(sb.api->Length(&sb) == 0);
    assert(strcmp(sb.api->CStr(&sb), "") == 0);

    /* Destroy */
    assert(sb.api->Destroy(&sb) == STATUS_CONST(SUCCESS));
    assert(sb.buffer == NULL);
    assert(sb.api->Capacity(&sb) == 0);
  }

  /* ------------------------------------------------------------
   * 2. Test via TEXT_BUILDER_FUNC direct macro calls
   * ------------------------------------------------------------ */
  {
    TStringBuilder sb = {0};
    assert(TEXT_BUILDER_FUNC(Init)(&sb) == STATUS_CONST(SUCCESS));
    assert(TEXT_BUILDER_FUNC(Create)(&sb, 16) == STATUS_CONST(SUCCESS));
    assert(TEXT_BUILDER_FUNC(Capacity)(&sb) >= 16);

    for (int i = 0; i < 50; ++i) {
      assert(TEXT_BUILDER_FUNC(AppendFormat)(&sb, "[%d]", i) == STATUS_CONST(SUCCESS));
    }
    assert(TEXT_BUILDER_FUNC(Length)(&sb) > 100);

    assert(TEXT_BUILDER_FUNC(ShrinkToFit)(&sb) == STATUS_CONST(SUCCESS));
    assert(TEXT_BUILDER_FUNC(Capacity)(&sb) == TEXT_BUILDER_FUNC(Length)(&sb));

    assert(TEXT_BUILDER_FUNC(Destroy)(&sb) == STATUS_CONST(SUCCESS));
  }

  return 0;
}
