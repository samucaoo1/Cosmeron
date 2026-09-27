#pragma once

#include "../Timezone/Timezone.h"
#include <stddef.h>

#define CHRONOMETRY_FORMAT_DATE_ISO_PROTOTYPE                        \
  inline static OPSTATUS CHRONOMETRY_FORMAT_FUNC(DateISO)(                      \
      CHRONOMETRY_TYPE(Date) date, char *buffer, size_t capacity)
#define CHRONOMETRY_FORMAT_DATETIME_ISO_PROTOTYPE                    \
  inline static OPSTATUS CHRONOMETRY_FORMAT_FUNC(DateTimeISO)(                  \
      CHRONOMETRY_TYPE(DateTime) value, char *buffer, size_t capacity)
#define CHRONOMETRY_PARSE_DATE_ISO_PROTOTYPE                         \
  inline static OPSTATUS CHRONOMETRY_FORMAT_FUNC(ParseDateISO)(                 \
      const char *text, CHRONOMETRY_TYPE(Date) *date)
#define CHRONOMETRY_PARSE_DATETIME_ISO_PROTOTYPE                     \
  inline static OPSTATUS CHRONOMETRY_FORMAT_FUNC(ParseDateTimeISO)(             \
      const char *text, CHRONOMETRY_TYPE(DateTime) *value)

#include "Impl/Format.impl"
