#pragma once

#include "../Text.space"
#include "../Types.h"
#include "../Unicode/Codepoint.h"

inline static size_t TEXT_UTF8_NS(EncodedLength)(TText_Char32 codepoint);
inline static TText_Encode_Result TEXT_UTF8_NS(Encode)(TText_Char32 codepoint,
                                                       unsigned char *output,
                                                       size_t capacity);
inline static TText_Decode_Result TEXT_UTF8_NS(Decode)(const unsigned char *input,
                                                       size_t size);
inline static bool TEXT_UTF8_NS(Validate)(const unsigned char *input,
                                         size_t size);
inline static TText_Measure_Result TEXT_UTF8_NS(Count)(
    const unsigned char *input, size_t size);

#include "Impl/UTF8.impl"
