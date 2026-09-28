#include "../../Cosmeron/Modules/Text/Text.h"

int main(void) {
  if (!TEXT_CODEPOINT_FUNC(IsValid)((TEXT_TYPE(TChar32))'A'))
    return 1;
  if (!TEXT_CODEPOINT_FUNC(IsASCII)((TEXT_TYPE(TChar32))'A'))
    return 2;
  if (!TEXT_CODEPOINT_FUNC(IsWhitespace)((TEXT_TYPE(TChar32))' '))
    return 3;
  if (TEXT_CODEPOINT_FUNC(IsScalar)((TEXT_TYPE(TChar32))UINT32_C(0xD800)))
    return 4;

  {
    unsigned char utf8[4] = {0};
    TEXT_TYPE(TChar32) decoded = 0;
    size_t encodedUnits = 0U;
    size_t decodedUnits = 0U;
    size_t codepoints = 0U;
    size_t consumed = 0U;
    size_t columns = 0U;

    if (TEXT_UTF8_FUNC(Encode)((TEXT_TYPE(TChar32))UINT32_C(0x1F600),
                              utf8, 4U, &encodedUnits) != STATUS_CONST(SUCCESS) ||
        encodedUnits != 4U)
      return 5;

    if (TEXT_UTF8_FUNC(Decode)(utf8, 4U, &decoded, &decodedUnits) !=
            STATUS_CONST(SUCCESS) ||
        decoded != (TEXT_TYPE(TChar32))UINT32_C(0x1F600) ||
        decodedUnits != 4U)
      return 6;

    if (!TEXT_UTF8_FUNC(Validate)(utf8, 4U))
      return 7;

    if (TEXT_UTF8_FUNC(Count)(utf8, 4U, &codepoints, &consumed) !=
            STATUS_CONST(SUCCESS) ||
        codepoints != 1U || consumed != 4U)
      return 8;

    if (TEXT_WIDTH_FUNC(UTF8)(utf8, 4U, &columns, &consumed) !=
            STATUS_CONST(SUCCESS) ||
        columns != 2U || consumed != 4U)
      return 9;
  }

  {
    TEXT_TYPE(TChar16) utf16[2] = {0};
    TEXT_TYPE(TChar32) decoded = 0;
    size_t encodedUnits = 0U;
    size_t decodedUnits = 0U;

    if (TEXT_UTF16_FUNC(Encode)((TEXT_TYPE(TChar32))UINT32_C(0x1F600),
                               utf16, 2U, &encodedUnits) !=
            STATUS_CONST(SUCCESS) ||
        encodedUnits != 2U)
      return 10;

    if (TEXT_UTF16_FUNC(Decode)(utf16, 2U, &decoded, &decodedUnits) !=
            STATUS_CONST(SUCCESS) ||
        decoded != (TEXT_TYPE(TChar32))UINT32_C(0x1F600) ||
        decodedUnits != 2U)
      return 11;

    if (!TEXT_UTF16_FUNC(Validate)(utf16, 2U))
      return 12;
  }

  {
    int width = -1;
    if (TEXT_WIDTH_FUNC(Codepoint)((TEXT_TYPE(TChar32))'A', &width) !=
            STATUS_CONST(SUCCESS) ||
        width != 1)
      return 13;
    if (TEXT_WIDTH_FUNC(Codepoint)((TEXT_TYPE(TChar32))UINT32_C(0x0301),
                                   &width) != STATUS_CONST(SUCCESS) ||
        width != 0)
      return 14;
    if (TEXT_WIDTH_FUNC(Codepoint)((TEXT_TYPE(TChar32))UINT32_C(0x754C),
                                   &width) != STATUS_CONST(SUCCESS) ||
        width != 2)
      return 15;
  }

  {
    static const unsigned char invalidUtf8[] = {0xC0U, 0x80U};
    TEXT_TYPE(TChar32) decoded = 0;
    size_t units = 0U;
    if (TEXT_UTF8_FUNC(Decode)(invalidUtf8, 2U, &decoded, &units) !=
        STATUS_CONST(INVALID_SEQUENCE))
      return 16;
  }

  return 0;
}
