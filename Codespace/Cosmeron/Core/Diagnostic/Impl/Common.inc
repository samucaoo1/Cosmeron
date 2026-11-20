#pragma once

#include "../../Module/Module.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* Internal Diagnostic Shared Infrastructure: FormatWriter Buffer Helper      */
/* ========================================================================== */

#define DIAGNOSTIC_INTERNAL_COL_NAME(NAME, VALUE) NAME
#define DIAGNOSTIC_INTERNAL_COL_VALUE(NAME, VALUE) VALUE
#define DIAGNOSTIC_INTERNAL_GET(FIELD, ROW)                                    \
  PP_OP_CAT2(DIAGNOSTIC_INTERNAL_COL_, FIELD) ROW

#define DIAGNOSTIC_INTERNAL_TABLE(X)                                           \
  X((DECIMAL_BASE, 10U))                                                       \
  X((DECIMAL_BUF_SIZE, 32U))                                                   \
  X((HEX_NIBBLE_MASK, 0xFU))                                                   \
  X((HEX_NIBBLE_BITS, 4U))                                                     \
  X((HEX_NIBBLES_PER_BYTE, 2U))                                                \
  X((HEX_PREFIX_RESERVE, 3U))

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
        ENUM(FormatConfig, DIAGNOSTIC_INTERNAL_TABLE),

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
          char temp[CNS(Diagnostic, Internal, DECIMAL_BUF_SIZE)];
          size_t idx = 0U;
          if (val == 0U) {
            MODULE_IFUNC(Diagnostic, FormatWriter_AppendChar)(w, '0');
            return;
          }
          while (val > 0U) {
            temp[idx++] =
                (char)('0' + (val % CNS(Diagnostic, Internal, DECIMAL_BASE)));
            val /= CNS(Diagnostic, Internal, DECIMAL_BASE);
          }
          while (idx > 0U) {
            MODULE_IFUNC(Diagnostic, FormatWriter_AppendChar)(w, temp[--idx]);
          }
        }),

        PROCEDURE_INLINE(FormatWriter_AppendHex,
                         (MODULE_ITYPE(Diagnostic, FormatWriter) * w,
                          uintptr_t val), {
          static const char hexChars[] = "0123456789abcdef";
          char temp[sizeof(uintptr_t) *
                        CNS(Diagnostic, Internal, HEX_NIBBLES_PER_BYTE) +
                    CNS(Diagnostic, Internal, HEX_PREFIX_RESERVE)];
          size_t idx = 0U;

          MODULE_IFUNC(Diagnostic, FormatWriter_AppendStr)(w, "0x");
          if (val == 0U) {
            MODULE_IFUNC(Diagnostic, FormatWriter_AppendChar)(w, '0');
            return;
          }
          while (val > 0U) {
            temp[idx++] =
                hexChars[val & CNS(Diagnostic, Internal, HEX_NIBBLE_MASK)];
            val >>= CNS(Diagnostic, Internal, HEX_NIBBLE_BITS);
          }
          while (idx > 0U) {
            MODULE_IFUNC(Diagnostic, FormatWriter_AppendChar)(w, temp[--idx]);
          }
        })
    )
);

/* EOF */

