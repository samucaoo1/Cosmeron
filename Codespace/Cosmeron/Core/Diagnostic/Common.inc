#pragma once

#include "../Module/Module.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* Internal Diagnostic Shared Infrastructure: FormatWriter Buffer Helper      */
/* ========================================================================== */

MODULE(DOMAIN(Core), Diagnostic,
    ISTRUCT(FormatWriter,
        FIELDS(
            char *buffer;
            size_t capacity;
            size_t written;
            bool overflow;
        )
    ),
    INTERNAL(
        PROCEDURE_INLINE(FormatWriter_Init,
                         (MODULE_ITYPE(Diagnostic, FormatWriter) * w,
                          char *buffer, size_t capacity), {
          w->buffer = buffer;
          w->capacity = capacity;
          w->written = 0U;
          w->overflow = false;
          if (buffer != NULL && capacity > 0U) {
            buffer[0] = '\0';
          }
        }),

        PROCEDURE_INLINE(FormatWriter_AppendChar,
                         (MODULE_ITYPE(Diagnostic, FormatWriter) * w, char c), {
          if (w->buffer != NULL && w->written + 1U < w->capacity) {
            w->buffer[w->written] = c;
            w->written++;
            w->buffer[w->written] = '\0';
          } else {
            w->overflow = true;
          }
        }),

        PROCEDURE_INLINE(FormatWriter_AppendStr,
                         (MODULE_ITYPE(Diagnostic, FormatWriter) * w,
                          const char *str), {
          if (str == NULL) {
            return;
          }
          while (*str != '\0') {
            MODULE_IFUNC(Diagnostic, FormatWriter_AppendChar)(w, *str++);
          }
        }),

        PROCEDURE_INLINE(FormatWriter_AppendUint,
                         (MODULE_ITYPE(Diagnostic, FormatWriter) * w,
                          uint64_t val), {
          char temp[32];
          size_t idx = 0U;
          if (val == 0U) {
            MODULE_IFUNC(Diagnostic, FormatWriter_AppendChar)(w, '0');
            return;
          }
          while (val > 0U) {
            temp[idx++] = (char)('0' + (val % 10U));
            val /= 10U;
          }
          while (idx > 0U) {
            MODULE_IFUNC(Diagnostic, FormatWriter_AppendChar)(w, temp[--idx]);
          }
        }),

        PROCEDURE_INLINE(FormatWriter_AppendHex,
                         (MODULE_ITYPE(Diagnostic, FormatWriter) * w,
                          uintptr_t val), {
          static const char hexChars[] = "0123456789abcdef";
          char temp[sizeof(uintptr_t) * 2U + 3U];
          size_t idx = 0U;

          MODULE_IFUNC(Diagnostic, FormatWriter_AppendStr)(w, "0x");
          if (val == 0U) {
            MODULE_IFUNC(Diagnostic, FormatWriter_AppendChar)(w, '0');
            return;
          }
          while (val > 0U) {
            temp[idx++] = hexChars[val & 0xFU];
            val >>= 4;
          }
          while (idx > 0U) {
            MODULE_IFUNC(Diagnostic, FormatWriter_AppendChar)(w, temp[--idx]);
          }
        })
    )
);

/* EOF */

