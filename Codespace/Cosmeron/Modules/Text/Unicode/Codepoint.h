#pragma once

#include "../Text.space"

inline static bool TEXT_CODEPOINT_NS(IsValid)(TText_Char32 codepoint);
inline static bool TEXT_CODEPOINT_NS(IsScalar)(TText_Char32 codepoint);
inline static bool TEXT_CODEPOINT_NS(IsASCII)(TText_Char32 codepoint);
inline static bool TEXT_CODEPOINT_NS(IsControl)(TText_Char32 codepoint);
inline static bool TEXT_CODEPOINT_NS(IsWhitespace)(TText_Char32 codepoint);

#include "Impl/Codepoint.impl"
