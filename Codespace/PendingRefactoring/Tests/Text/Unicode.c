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
    static const unsigned char incompleteUtf8[] = {0xF0U, 0x9FU};
    TEXT_TYPE(TChar32) decoded = (TEXT_TYPE(TChar32))UINT32_C(0x55AA);
    size_t units = 77U;
    size_t count = 88U;
    size_t columns = 99U;

    if (TEXT_UTF8_FUNC(Decode)(invalidUtf8, 2U, &decoded, &units) !=
            STATUS_CONST(INVALID_SEQUENCE) ||
        decoded != (TEXT_TYPE(TChar32))UINT32_C(0x55AA) || units != 77U)
      return 16;
    if (TEXT_UTF8_FUNC(Decode)(incompleteUtf8, 2U, &decoded, &units) !=
            STATUS_CONST(INCOMPLETE_SEQUENCE) ||
        decoded != (TEXT_TYPE(TChar32))UINT32_C(0x55AA) || units != 77U)
      return 17;
    if (TEXT_UTF8_FUNC(Count)(invalidUtf8, 2U, &count, &units) !=
            STATUS_CONST(INVALID_SEQUENCE) ||
        count != 88U || units != 77U)
      return 18;
    if (TEXT_WIDTH_FUNC(UTF8)(invalidUtf8, 2U, &columns, &units) !=
            STATUS_CONST(INVALID_SEQUENCE) ||
        columns != 99U || units != 77U)
      return 19;

    {
      unsigned char small[1] = {0xEEU};
      units = 77U;
      if (TEXT_UTF8_FUNC(Encode)(
              (TEXT_TYPE(TChar32))UINT32_C(0x1F600), small, 1U, &units) !=
              STATUS_CONST(INSUFFICIENT_SPACE) ||
          units != 77U || small[0] != 0xEEU)
        return 20;
    }
  }

  {
    TEXT_TYPE(TChar16) invalidUtf16[] = {(TEXT_TYPE(TChar16))UINT16_C(0xDC00)};
    TEXT_TYPE(TChar32) decoded = (TEXT_TYPE(TChar32))UINT32_C(0xAA55);
    size_t units = 66U;
    if (TEXT_UTF16_FUNC(Decode)(invalidUtf16, 1U, &decoded, &units) !=
            STATUS_CONST(INVALID_SEQUENCE) ||
        decoded != (TEXT_TYPE(TChar32))UINT32_C(0xAA55) || units != 66U)
      return 21;
  }

  return 0;
}
