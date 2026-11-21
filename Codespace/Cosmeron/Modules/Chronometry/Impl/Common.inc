#pragma once

#include <stdint.h>

#include "../../../Core/Algorithm/Comparison.h"
#include "../../../Core/Condition/Status.h"
#include "../../../Core/Module/Module.h"

#if defined(CHRONOMETRY_DISABLE_FUNCTION_TABLE) &&                             \
    !defined(COSMERON_DISABLE_FT_Chronometry)
#define COSMERON_DISABLE_FT_Chronometry 1
#endif
#if defined(CHRONOMETRY_DURATION_DISABLE_FUNCTION_TABLE) &&                    \
    !defined(COSMERON_DISABLE_FT_Chronometry_Duration)
#define COSMERON_DISABLE_FT_Chronometry_Duration 1
#endif
#if defined(CHRONOMETRY_CLOCK_DISABLE_FUNCTION_TABLE) &&                       \
    !defined(COSMERON_DISABLE_FT_Chronometry_Clock)
#define COSMERON_DISABLE_FT_Chronometry_Clock 1
#endif
#if defined(CHRONOMETRY_TIMER_DISABLE_FUNCTION_TABLE) &&                       \
    !defined(COSMERON_DISABLE_FT_Chronometry_Timer)
#define COSMERON_DISABLE_FT_Chronometry_Timer 1
#endif
#if defined(CHRONOMETRY_INSTANT_DISABLE_FUNCTION_TABLE) &&                     \
    !defined(COSMERON_DISABLE_FT_Chronometry_Instant)
#define COSMERON_DISABLE_FT_Chronometry_Instant 1
#endif
#if defined(CHRONOMETRY_CALENDAR_DISABLE_FUNCTION_TABLE) &&                    \
    !defined(COSMERON_DISABLE_FT_Chronometry_Calendar)
#define COSMERON_DISABLE_FT_Chronometry_Calendar 1
#endif
#if defined(CHRONOMETRY_TIMESTAMP_DISABLE_FUNCTION_TABLE) &&                   \
    !defined(COSMERON_DISABLE_FT_Chronometry_Timestamp)
#define COSMERON_DISABLE_FT_Chronometry_Timestamp 1
#endif
#if defined(CHRONOMETRY_EPOCH_DISABLE_FUNCTION_TABLE) &&                       \
    !defined(COSMERON_DISABLE_FT_Chronometry_Epoch)
#define COSMERON_DISABLE_FT_Chronometry_Epoch 1
#endif
#if defined(CHRONOMETRY_TIMEZONE_DISABLE_FUNCTION_TABLE) &&                    \
    !defined(COSMERON_DISABLE_FT_Chronometry_Timezone)
#define COSMERON_DISABLE_FT_Chronometry_Timezone 1
#endif
#if defined(CHRONOMETRY_FORMAT_DISABLE_FUNCTION_TABLE) &&                      \
    !defined(COSMERON_DISABLE_FT_Chronometry_Format)
#define COSMERON_DISABLE_FT_Chronometry_Format 1
#endif

/* ========================================================================== */
/* Shared Internal Arithmetic Helpers                                         */
/* ========================================================================== */

MODULE(DOMAIN(Modules), Chronometry,
    INTERNAL(
        FUNCTION_INLINE(int64_t, FloorDiv, (int64_t value, int64_t divisor), {
          int64_t quotient = value / divisor;
          int64_t remainder = value % divisor;
          return quotient - (remainder < 0);
        })
    )
);

/* EOF */
