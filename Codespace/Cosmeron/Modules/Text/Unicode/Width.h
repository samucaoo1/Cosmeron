#pragma once

#include "../Encoding/UTF8.h"

inline static int TEXT_WIDTH_NS(Codepoint)(TText_Char32 codepoint);
inline static TText_Measure_Result TEXT_WIDTH_NS(UTF8)(
    const unsigned char *input, size_t size);

#include "Impl/Width.impl"
