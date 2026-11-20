#pragma once

#include "../../../../Core/Algorithm/Comparison.h"
#include "../../../../Core/Condition/Status.h"
#include "../../../../Core/Memory/Alloc.h"
#include "../../../../Core/Module/Module.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(TEXT_DISABLE_FUNCTION_TABLE) && !defined(COSMERON_DISABLE_FT_Text)
#define COSMERON_DISABLE_FT_Text 1
#endif
#if defined(TEXT_CSTRING_DISABLE_FUNCTION_TABLE) &&                            \
    !defined(COSMERON_DISABLE_FT_Text_CString)
#define COSMERON_DISABLE_FT_Text_CString 1
#endif

MODULE(DOMAIN(Modules), Text,
    PACKAGE(CString,
        VARIANT(Length,
                FUNCTION(size_t, (const char *str)),
                FUNCTION(size_t, Bounded, (const char *str, size_t maxLen))
        ),
        PREDICATE(IsEmpty, (const char *str)),
        VARIANT(IsAscii,
                PREDICATE((const char *str)),
                PREDICATE(Bounded, (const char *str, size_t maxLen))
        ),
        VARIANT(Copy,
                OPERATION((char *dest, size_t destCapacity, const char *src,
                           size_t *outLength)),
                OPERATION(Bounded,
                          (char *dest, size_t destCapacity, const char *src,
                           size_t maxSourceChars, size_t *outLength))
        ),
        VARIANT(Concat,
                OPERATION((char *dest, size_t destCapacity, const char *src,
                           size_t *outLength)),
                OPERATION(Bounded,
                          (char *dest, size_t destCapacity, const char *src,
                           size_t maxSourceChars, size_t *outLength))
        ),
        VARIANT(Compare,
                COMPARISON((const char *left, const char *right)),
                COMPARISON(Bounded,
                           (const char *left, const char *right, size_t maxLen)),
                COMPARISON(IgnoreCase, (const char *left, const char *right)),
                COMPARISON(IgnoreCaseBounded,
                           (const char *left, const char *right, size_t maxLen))
        ),
        VARIANT(Equal,
                PREDICATE((const char *left, const char *right)),
                PREDICATE(Bounded,
                          (const char *left, const char *right, size_t maxLen)),
                PREDICATE(IgnoreCase, (const char *left, const char *right)),
                PREDICATE(IgnoreCaseBounded,
                          (const char *left, const char *right, size_t maxLen))
        ),
        VARIANT(StartsWith,
                PREDICATE((const char *str, const char *prefix)),
                PREDICATE(Bounded,
                          (const char *str, size_t strLen, const char *prefix)),
                PREDICATE(IgnoreCase, (const char *str, const char *prefix))
        ),
        VARIANT(EndsWith,
                PREDICATE((const char *str, const char *suffix)),
                PREDICATE(Bounded,
                          (const char *str, size_t strLen, const char *suffix)),
                PREDICATE(IgnoreCase, (const char *str, const char *suffix))
        ),
        VARIANT(FindChar,
                OPERATION((const char *str, char ch, size_t *outIndex)),
                OPERATION(Bounded,
                          (const char *str, size_t maxLen, char ch,
                           size_t *outIndex))
        ),
        VARIANT(FindLastChar,
                OPERATION((const char *str, char ch, size_t *outIndex)),
                OPERATION(Bounded,
                          (const char *str, size_t maxLen, char ch,
                           size_t *outIndex))
        ),
        VARIANT(FindSubstring,
                OPERATION((const char *haystack, const char *needle,
                           size_t *outIndex)),
                OPERATION(Bounded,
                          (const char *haystack, size_t maxHaystackLen,
                           const char *needle, size_t *outIndex))
        ),
        OPERATION(Span,
                  (const char *str, const char *accept, size_t *outLength)),
        OPERATION(ComplementSpan,
                  (const char *str, const char *reject, size_t *outLength)),
        VARIANT(Trim,
                OPERATION((const char *str, const char *charsToTrim,
                           char *output, size_t capacity, size_t *outLength)),
                OPERATION(Start,
                          (const char *str, const char *charsToTrim,
                           char *output, size_t capacity, size_t *outLength)),
                OPERATION(End,
                          (const char *str, const char *charsToTrim,
                           char *output, size_t capacity, size_t *outLength))
        ),
        VARIANT(CountChar,
                OPERATION((const char *str, char ch, size_t *outCount)),
                OPERATION(Bounded,
                          (const char *str, size_t maxLen, char ch,
                           size_t *outCount))
        ),
        VARIANT(ReplaceChar,
                OPERATION((char *str, char from, char to, size_t *outCount)),
                OPERATION(Bounded,
                          (char *str, size_t maxLen, char from, char to,
                           size_t *outCount))
        ),
        VARIANT(Duplicate,
                OPERATION((const char *src, char **outDest)),
                OPERATION(Bounded,
                          (const char *src, size_t maxLen, char **outDest))
        ),
        PROCEDURE(Free, (char *str))
    )
);

#include "Impl/CString.impl"

/* EOF */
