#pragma once

#include "../../../../Core/Algorithm/Comparison.h"
#include "../../../../Core/Error/Status.h"
#include "../../../../Core/Memory/Alloc.h"
#include "../../../../Core/Module/Package.h"
#include "../../Text.space"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(TEXT_DISABLE_FUNCTION_TABLE) ||                                    \
    defined(TEXT_CSTRING_DISABLE_FUNCTION_TABLE)
#ifndef PACKAGE_DISABLE_FUNCTION_TABLE
#define PACKAGE_DISABLE_FUNCTION_TABLE
#endif
#endif

PACKAGE(Text_CString,
    FUNCTION(size_t, Length, (const char *str)),
    FUNCTION(size_t, LengthBounded, (const char *str, size_t maxLen)),
    FUNCTION(bool, IsEmpty, (const char *str)),
    FUNCTION(bool, IsAscii, (const char *str)),
    FUNCTION(bool, IsAsciiBounded, (const char *str, size_t maxLen)),
    FUNCTION(OPSTATUS, Copy,
             (char *dest, size_t destCapacity, const char *src,
              size_t *outLength)),
    FUNCTION(OPSTATUS, CopyBounded,
             (char *dest, size_t destCapacity, const char *src,
              size_t maxSourceChars, size_t *outLength)),
    FUNCTION(OPSTATUS, Concat,
             (char *dest, size_t destCapacity, const char *src,
              size_t *outLength)),
    FUNCTION(OPSTATUS, ConcatBounded,
             (char *dest, size_t destCapacity, const char *src,
              size_t maxSourceChars, size_t *outLength)),
    FUNCTION(CMPOUT, Compare, (const char *left, const char *right)),
    FUNCTION(CMPOUT, CompareBounded,
             (const char *left, const char *right, size_t maxLen)),
    FUNCTION(CMPOUT, CompareIgnoreCase, (const char *left, const char *right)),
    FUNCTION(CMPOUT, CompareIgnoreCaseBounded,
             (const char *left, const char *right, size_t maxLen)),
    FUNCTION(bool, Equal, (const char *left, const char *right)),
    FUNCTION(bool, EqualBounded,
             (const char *left, const char *right, size_t maxLen)),
    FUNCTION(bool, EqualIgnoreCase, (const char *left, const char *right)),
    FUNCTION(bool, EqualIgnoreCaseBounded,
             (const char *left, const char *right, size_t maxLen)),
    FUNCTION(bool, StartsWith, (const char *str, const char *prefix)),
    FUNCTION(bool, StartsWithBounded,
             (const char *str, size_t strLen, const char *prefix)),
    FUNCTION(bool, StartsWithIgnoreCase, (const char *str, const char *prefix)),
    FUNCTION(bool, EndsWith, (const char *str, const char *suffix)),
    FUNCTION(bool, EndsWithBounded,
             (const char *str, size_t strLen, const char *suffix)),
    FUNCTION(bool, EndsWithIgnoreCase, (const char *str, const char *suffix)),
    FUNCTION(OPSTATUS, FindChar, (const char *str, char ch, size_t *outIndex)),
    FUNCTION(OPSTATUS, FindCharBounded,
             (const char *str, size_t maxLen, char ch, size_t *outIndex)),
    FUNCTION(OPSTATUS, FindLastChar,
             (const char *str, char ch, size_t *outIndex)),
    FUNCTION(OPSTATUS, FindLastCharBounded,
             (const char *str, size_t maxLen, char ch, size_t *outIndex)),
    FUNCTION(OPSTATUS, FindSubstring,
             (const char *haystack, const char *needle, size_t *outIndex)),
    FUNCTION(OPSTATUS, FindSubstringBounded,
             (const char *haystack, size_t maxHaystackLen, const char *needle,
              size_t *outIndex)),
    FUNCTION(OPSTATUS, Span,
             (const char *str, const char *accept, size_t *outLength)),
    FUNCTION(OPSTATUS, ComplementSpan,
             (const char *str, const char *reject, size_t *outLength)),
    FUNCTION(OPSTATUS, Trim,
             (const char *str, const char *charsToTrim, char *output,
              size_t capacity, size_t *outLength)),
    FUNCTION(OPSTATUS, TrimStart,
             (const char *str, const char *charsToTrim, char *output,
              size_t capacity, size_t *outLength)),
    FUNCTION(OPSTATUS, TrimEnd,
             (const char *str, const char *charsToTrim, char *output,
              size_t capacity, size_t *outLength)),
    FUNCTION(OPSTATUS, CountChar, (const char *str, char ch, size_t *outCount)),
    FUNCTION(OPSTATUS, CountCharBounded,
             (const char *str, size_t maxLen, char ch, size_t *outCount)),
    FUNCTION(OPSTATUS, ReplaceChar,
             (char *str, char from, char to, size_t *outCount)),
    FUNCTION(OPSTATUS, ReplaceCharBounded,
             (char *str, size_t maxLen, char from, char to, size_t *outCount)),
    FUNCTION(OPSTATUS, Duplicate, (const char *src, char **outDest)),
    FUNCTION(OPSTATUS, DuplicateBounded,
             (const char *src, size_t maxLen, char **outDest)),
    FUNCTION(void, Free, (char *str))
);

#include "Impl/CString.impl"
