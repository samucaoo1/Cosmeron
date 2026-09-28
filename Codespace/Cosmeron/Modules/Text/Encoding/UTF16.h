#pragma once

#include "../Text.space"
#include "../Types.h"
#include "../Unicode/Codepoint.h"

inline static size_t TEXT_UTF16_NS(EncodedLength)(TText_Char32 codepoint);
inline static TText_Encode_Result TEXT_UTF16_NS(Encode)(TText_Char32 codepoint,
                                                        TText_Char16 *output,
                                                        size_t capacity);
inline static TText_Decode_Result TEXT_UTF16_NS(Decode)(const TText_Char16 *input,
                                                        size_t size);
inline static bool TEXT_UTF16_NS(Validate)(const TText_Char16 *input, size_t size);

#include "Impl/UTF16.impl"
