#pragma once

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdint.h>

#include "../../../Core/Algorithm/Comparison.h"
#include "../../../Core/Condition/Status.h"
#include "../../../Core/Module/Module.h"

#if defined(CHRONO_DISABLE_FUNCTION_TABLE) &&                                  \
    !defined(COSMERON_DISABLE_FT_Chrono)
#define COSMERON_DISABLE_FT_Chrono 1
#endif
#if defined(CHRONO_MATH_DISABLE_FUNCTION_TABLE) &&                             \
    !defined(COSMERON_DISABLE_FT_Chrono_Math)
#define COSMERON_DISABLE_FT_Chrono_Math 1
#endif
#if defined(CHRONO_CLOCK_DISABLE_FUNCTION_TABLE) &&                            \
    !defined(COSMERON_DISABLE_FT_Chrono_Clock)
#define COSMERON_DISABLE_FT_Chrono_Clock 1
#endif
#if defined(CHRONO_CALENDAR_DISABLE_FUNCTION_TABLE) &&                         \
    !defined(COSMERON_DISABLE_FT_Chrono_Calendar)
#define COSMERON_DISABLE_FT_Chrono_Calendar 1
#endif
#if defined(CHRONO_CIVIL_DISABLE_FUNCTION_TABLE) &&                            \
    !defined(COSMERON_DISABLE_FT_Chrono_Civil)
#define COSMERON_DISABLE_FT_Chrono_Civil 1
#endif
#if defined(CHRONO_TIMEZONE_DISABLE_FUNCTION_TABLE) &&                         \
    !defined(COSMERON_DISABLE_FT_Chrono_Timezone)
#define COSMERON_DISABLE_FT_Chrono_Timezone 1
#endif
#if defined(CHRONO_FORMAT_DISABLE_FUNCTION_TABLE) &&                           \
    !defined(COSMERON_DISABLE_FT_Chrono_Format)
#define COSMERON_DISABLE_FT_Chrono_Format 1
#endif

/* ========================================================================== */
/* Shared Internal Arithmetic Helpers                                         */
/* ========================================================================== */

MODULE(DOMAIN(Modules), Chrono,
    INTERNAL(
        FUNCTION_INLINE(int64_t, FloorDiv, (int64_t value, int64_t divisor), {
          int64_t quotient = value / divisor;
          int64_t remainder = value % divisor;
          return quotient - (remainder < 0);
        }),

        FUNCTION_INLINE(int64_t, FloorMod, (int64_t value, int64_t divisor), {
          return value -
                 MODULE_IFUNC(Chrono, FloorDiv)(value, divisor) * divisor;
        })
    )
);

/* EOF */
