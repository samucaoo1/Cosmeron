#include "../../Cosmeron/Modules/Text/Text.h"

int main(void) {
  if (!Text_Codepoint_IsValid((TText_Char32)'A'))
    return 1;
  if (!Text_Codepoint_IsASCII((TText_Char32)'A'))
    return 2;
  if (!Text_Codepoint_IsWhitespace((TText_Char32)' '))
    return 3;
  if (Text_Codepoint_IsScalar((TText_Char32)UINT32_C(0xD800)))
    return 4;

  {
    unsigned char utf8[4] = {0};
    TText_Encode_Result encoded =
        Text_UTF8_Encode((TText_Char32)UINT32_C(0x1F600), utf8, 4U);
    TText_Decode_Result decoded;
    TText_Measure_Result count;
    TText_Measure_Result width;

    if (encoded.error != TEXT_ERROR_NONE || encoded.units != 4U)
      return 5;
    decoded = Text_UTF8_Decode(utf8, 4U);
    if (decoded.error != TEXT_ERROR_NONE ||
        decoded.codepoint != (TText_Char32)UINT32_C(0x1F600) ||
        decoded.units != 4U)
      return 6;
    if (!Text_UTF8_Validate(utf8, 4U))
      return 7;
    count = Text_UTF8_Count(utf8, 4U);
    if (count.error != TEXT_ERROR_NONE || count.columns != 1U ||
        count.units != 4U)
      return 8;
    width = Text_Width_UTF8(utf8, 4U);
    if (width.error != TEXT_ERROR_NONE || width.columns != 2U ||
        width.units != 4U)
      return 9;
  }

  {
    TText_Char16 utf16[2] = {0};
    TText_Encode_Result encoded =
        Text_UTF16_Encode((TText_Char32)UINT32_C(0x1F600), utf16, 2U);
    TText_Decode_Result decoded;

    if (encoded.error != TEXT_ERROR_NONE || encoded.units != 2U)
      return 10;
    decoded = Text_UTF16_Decode(utf16, 2U);
    if (decoded.error != TEXT_ERROR_NONE ||
        decoded.codepoint != (TText_Char32)UINT32_C(0x1F600) ||
        decoded.units != 2U)
      return 11;
    if (!Text_UTF16_Validate(utf16, 2U))
      return 12;
  }

  if (Text_Width_Codepoint((TText_Char32)'A') != 1)
    return 13;
  if (Text_Width_Codepoint((TText_Char32)UINT32_C(0x0301)) != 0)
    return 14;
  if (Text_Width_Codepoint((TText_Char32)UINT32_C(0x754C)) != 2)
    return 15;

  {
    static const unsigned char invalidUtf8[] = {0xC0U, 0x80U};
    TText_Decode_Result decoded = Text_UTF8_Decode(invalidUtf8, 2U);
    if (decoded.error != TEXT_ERROR_INVALID_SEQUENCE)
      return 16;
  }

  return 0;
}
